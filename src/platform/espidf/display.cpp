#include "zyngine/hal/display.hpp"
#include "zyngine/hal/gpio.hpp"
#include "zyngine/hal/input.hpp"
#include "zyngine/hal/log.hpp"
#include "zyngine/hal/touch_calib.hpp"
#include "zyngine/graphics/color.hpp"
#include "zyngine/graphics/renderer.hpp"

#include "esp32_ili9341_32/board.hpp"

#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_err.h"
#include "esp_heap_caps.h"
#include "esp_lcd_ili9341.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "nvs.h"
#include "nvs_flash.h"

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <vector>

namespace zyngine::hal {

constexpr char kNvsNs[] = "zyn_touch";

namespace {

constexpr spi_host_device_t kSpiHost = SPI3_HOST;
constexpr int kLcdMhz = 27;
constexpr int kTouchHz = 2500000;
constexpr uint8_t kXptCmdX = 0xD0;
constexpr uint8_t kXptCmdY = 0x90;
constexpr uint8_t kXptCmdZ1 = 0xB0;
constexpr int kTouchPressureMin = 40;  // edges of resistive panels read soft

bool onColorTransDone(esp_lcd_panel_io_handle_t, esp_lcd_panel_io_event_data_t*, void* ctx) {
    xSemaphoreGive(static_cast<SemaphoreHandle_t>(ctx));
    return false;
}

uint32_t rowHash(const uint16_t* row, int width) {
    uint32_t h = 2166136261u;
    for (int x = 0; x < width; ++x) {
        h ^= row[x];
        h *= 16777619u;
    }
    return h;
}

// Extrapolate past the two cal targets so inset crosses still cover the full screen.
float mapAxis(int raw, int lo, int hi, float outLo, float outHi, float clampLo, float clampHi) {
    if (hi == lo) {
        return outLo;
    }
    const float t = static_cast<float>(raw - lo) / static_cast<float>(hi - lo);
    return std::clamp(outLo + t * (outHi - outLo), clampLo, clampHi);
}

PointerState g_pointer{};

}  // namespace

class EspIdfDisplay final : public Display {
public:
    bool begin(int width, int height) override {
        width_ = width;
        height_ = height;
        const auto board = zyngine::board::esp32Ili9341_32Board();
        touchCal_.swapXy = (board.display.rotation % 2) == 1;

        flushSem_ = xSemaphoreCreateBinary();
        if (!flushSem_) {
            Log::error("Display", "flush semaphore alloc failed");
            return false;
        }

        spi_bus_config_t buscfg = {};
        buscfg.sclk_io_num = board.pins.lcdSck;
        buscfg.mosi_io_num = board.pins.lcdMosi;
        buscfg.miso_io_num = board.pins.lcdMiso;
        buscfg.quadwp_io_num = -1;
        buscfg.quadhd_io_num = -1;
        buscfg.max_transfer_sz = width_ * static_cast<int>(sizeof(uint16_t));

        esp_err_t err = spi_bus_initialize(kSpiHost, &buscfg, SPI_DMA_CH_AUTO);
        if (err != ESP_OK) {
            Log::error("Display", "spi_bus_initialize failed: %s", esp_err_to_name(err));
            return false;
        }

        esp_lcd_panel_io_handle_t io = nullptr;
        esp_lcd_panel_io_spi_config_t io_config = {};
        io_config.cs_gpio_num = static_cast<gpio_num_t>(board.pins.lcdCs);
        io_config.dc_gpio_num = static_cast<gpio_num_t>(board.pins.lcdDc);
        io_config.spi_mode = 0;
        io_config.pclk_hz = kLcdMhz * 1000 * 1000;
        io_config.trans_queue_depth = 1;
        io_config.lcd_cmd_bits = 8;
        io_config.lcd_param_bits = 8;
        io_config.on_color_trans_done = onColorTransDone;
        io_config.user_ctx = flushSem_;
        err = esp_lcd_new_panel_io_spi(kSpiHost, &io_config, &io);
        if (err != ESP_OK) {
            Log::error("Display", "panel_io_spi failed: %s", esp_err_to_name(err));
            return false;
        }
        io_ = io;

        esp_lcd_panel_handle_t panel = nullptr;
        esp_lcd_panel_dev_config_t panel_config = {};
        panel_config.reset_gpio_num = static_cast<gpio_num_t>(board.pins.lcdRst);
        // MADCTL BGR: panel swaps R/B vs our RGB565 packing (standard for these modules).
        panel_config.rgb_ele_order = LCD_RGB_ELEMENT_ORDER_BGR;
        panel_config.bits_per_pixel = 16;
        err = esp_lcd_new_panel_ili9341(io_, &panel_config, &panel);
        if (err != ESP_OK) {
            Log::error("Display", "ili9341 panel failed: %s", esp_err_to_name(err));
            return false;
        }
        panel_ = panel;

        ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_));
        ESP_ERROR_CHECK(esp_lcd_panel_init(panel_));
        const bool landscape = (board.display.rotation % 2) == 1;
        ESP_ERROR_CHECK(esp_lcd_panel_swap_xy(panel_, landscape));
        ESP_ERROR_CHECK(esp_lcd_panel_mirror(panel_, board.display.mirrorX, board.display.mirrorY));
        ESP_ERROR_CHECK(esp_lcd_panel_invert_color(panel_, false));
        ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_, true));

        if (board.pins.backlight >= 0) {
            Gpio::setMode(board.pins.backlight, PinMode::Output);
            Gpio::write(board.pins.backlight, PinLevel::High);
        }

        if (board.pins.touchCs >= 0) {
            spi_device_interface_config_t touch_cfg = {};
            touch_cfg.clock_speed_hz = kTouchHz;
            touch_cfg.mode = 0;
            touch_cfg.spics_io_num = board.pins.touchCs;
            touch_cfg.queue_size = 1;
            err = spi_bus_add_device(kSpiHost, &touch_cfg, &touch_);
            if (err != ESP_OK) {
                Log::error("Display", "touch spi device failed: %s", esp_err_to_name(err));
                touch_ = nullptr;
            }
        }

        // SPI LCD wants MSB-first RGB565; ESP32 memory is LE — swap in a DMA line buffer.
        lineBuf_ = static_cast<uint16_t*>(
            heap_caps_malloc(static_cast<size_t>(width_) * sizeof(uint16_t), MALLOC_CAP_DMA));
        if (!lineBuf_) {
            Log::error("Display", "DMA line buffer alloc failed");
            return false;
        }
        rowHash_.assign(static_cast<size_t>(height_), 0);
        firstPresent_ = true;
        g_active = this;
        Log::info("Display", "esp_lcd ILI9341 %dx%d DMA SPI", width_, height_);
        return true;
    }

    void end() override {
        if (g_active == this) {
            g_active = nullptr;
        }
        if (touch_) {
            spi_bus_remove_device(touch_);
            touch_ = nullptr;
        }
        if (panel_) {
            esp_lcd_panel_del(panel_);
            panel_ = nullptr;
        }
        if (io_) {
            esp_lcd_panel_io_del(io_);
            io_ = nullptr;
        }
        spi_bus_free(kSpiHost);
        if (lineBuf_) {
            heap_caps_free(lineBuf_);
            lineBuf_ = nullptr;
        }
        if (flushSem_) {
            vSemaphoreDelete(flushSem_);
            flushSem_ = nullptr;
        }
    }

    int width() const override { return width_; }
    int height() const override { return height_; }

    void present(const uint16_t* pixels, int width, int height) override {
        if (!panel_ || !pixels || !lineBuf_) {
            return;
        }
        for (int y = 0; y < height; ++y) {
            const uint16_t* row = pixels + y * width;
            const uint32_t h = rowHash(row, width);
            if (!firstPresent_ && h == rowHash_[static_cast<size_t>(y)]) {
                continue;
            }
            rowHash_[static_cast<size_t>(y)] = h;
            for (int x = 0; x < width; ++x) {
                lineBuf_[x] = __builtin_bswap16(row[x]);
            }
            esp_lcd_panel_draw_bitmap(panel_, 0, y, width, y + 1, lineBuf_);
            xSemaphoreTake(flushSem_, portMAX_DELAY);
        }
        firstPresent_ = false;
    }

    void setBacklight(bool on) override {
        const auto board = zyngine::board::esp32Ili9341_32Board();
        if (board.pins.backlight >= 0) {
            Gpio::write(board.pins.backlight, on ? PinLevel::High : PinLevel::Low);
        }
    }

    bool shouldContinue() const override { return true; }

    void pollEvents() override {
        const bool wasDown = g_pointer.down;
        float x = g_pointer.x;
        float y = g_pointer.y;
        bool down = false;

        int rawX = 0;
        int rawY = 0;
        if (sampleRaw(&rawX, &rawY)) {
            int ax = touchCal_.swapXy ? rawY : rawX;
            int ay = touchCal_.swapXy ? rawX : rawY;
            const float xLo = static_cast<float>(touchCal_.sx0);
            const float xHi = static_cast<float>(touchCal_.sx1 != 0 ? touchCal_.sx1 : width_ - 1);
            const float yLo = static_cast<float>(touchCal_.sy0);
            const float yHi = static_cast<float>(touchCal_.sy1 != 0 ? touchCal_.sy1 : height_ - 1);
            x = mapAxis(ax, touchCal_.x0, touchCal_.x1, xLo, xHi, 0.f,
                        static_cast<float>(width_ - 1));
            y = mapAxis(ay, touchCal_.y0, touchCal_.y1, yLo, yHi, 0.f,
                        static_cast<float>(height_ - 1));
            down = true;
        }

        g_pointer.x = x;
        g_pointer.y = y;
        g_pointer.down = down;
        g_pointer.pressed = down && !wasDown;
        g_pointer.released = !down && wasDown;
    }

    bool sampleRaw(int* outX, int* outY) {
        if (!touch_ || !outX || !outY) {
            return false;
        }
        if (readXpt(kXptCmdZ1) < kTouchPressureMin) {
            return false;
        }
        *outX = readXpt(kXptCmdX);
        *outY = readXpt(kXptCmdY);
        return true;
    }

    void setCal(const TouchCalData& cal) { touchCal_ = cal; }
    TouchCalData cal() const { return touchCal_; }

    static EspIdfDisplay* g_active;

private:
    int readXpt(uint8_t cmd) {
        uint8_t tx[3] = {cmd, 0, 0};
        uint8_t rx[3] = {};
        spi_transaction_t t = {};
        t.length = 24;
        t.tx_buffer = tx;
        t.rx_buffer = rx;
        if (spi_device_transmit(touch_, &t) != ESP_OK) {
            return 0;
        }
        return (static_cast<int>(rx[1]) << 4) | (static_cast<int>(rx[2]) >> 4);
    }

    int width_ = 0;
    int height_ = 0;
    esp_lcd_panel_io_handle_t io_ = nullptr;
    esp_lcd_panel_handle_t panel_ = nullptr;
    spi_device_handle_t touch_ = nullptr;
    SemaphoreHandle_t flushSem_ = nullptr;
    uint16_t* lineBuf_ = nullptr;
    std::vector<uint32_t> rowHash_;
    bool firstPresent_ = true;
    TouchCalData touchCal_{};
};

EspIdfDisplay* EspIdfDisplay::g_active = nullptr;

namespace {

bool ensureNvs() {
    static bool once = false;
    if (once) {
        return true;
    }
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        err = nvs_flash_init();
    }
    once = (err == ESP_OK);
    return once;
}

bool waitTouchSample(EspIdfDisplay* d, int* rx, int* ry) {
    bool seen = false;
    int skip = 0;
    int64_t sx = 0;
    int64_t sy = 0;
    int n = 0;
    while (true) {
        int x = 0;
        int y = 0;
        const bool down = d->sampleRaw(&x, &y);
        if (down) {
            if (skip < 3) {
                ++skip;  // ponytail: drop contact bounce
            } else {
                sx += x;
                sy += y;
                ++n;
                seen = true;
            }
        } else if (seen && n >= 8) {
            *rx = static_cast<int>(sx / n);
            *ry = static_cast<int>(sy / n);
            vTaskDelay(pdMS_TO_TICKS(80));
            return true;
        } else if (!down) {
            skip = 0;
            sx = sy = n = 0;
            seen = false;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void drawCross(Renderer& r, int cx, int cy, uint16_t color) {
    r.drawLine(cx - 18, cy, cx + 18, cy, color);
    r.drawLine(cx, cy - 18, cx, cy + 18, color);
    r.drawRect(cx - 6, cy - 6, 12, 12, color);
}

void drawCalFrame(Renderer& r, const char* label, int cx, int cy, uint16_t fg, uint16_t dim,
                  uint16_t bg) {
    r.beginFrame();
    r.clear(bg);
    r.drawText(8, 8, "TOUCH CALIBRATION", fg, 2);
    r.drawText(8, 28, label, dim, 1);
    r.drawText(8, 40, "Firm press on the cross, then lift", dim, 1);
    drawCross(r, cx, cy, fg);
    r.endFrame();
}

}  // namespace

Display* createDisplay() { return new EspIdfDisplay(); }

PointerState Input::pointer() { return g_pointer; }

bool Input::secondaryDown() { return false; }

void Input::scrollDelta(float* dx, float* dy) {
    if (dx) *dx = 0.f;
    if (dy) *dy = 0.f;
}

bool TouchCalib::load(TouchCalData* out) {
    if (!out || !ensureNvs()) {
        return false;
    }
    nvs_handle_t h = 0;
    if (nvs_open(kNvsNs, NVS_READONLY, &h) != ESP_OK) {
        return false;
    }
    int32_t x0 = 0, y0 = 0, x1 = 0, y1 = 0, sx0 = 0, sy0 = 0, sx1 = 0, sy1 = 0, swap = 0, magic = 0;
    // 'TCP3' — inset targets + screen endpoints
    const bool ok = nvs_get_i32(h, "magic", &magic) == ESP_OK && magic == 0x54435033 &&
                    nvs_get_i32(h, "x0", &x0) == ESP_OK && nvs_get_i32(h, "y0", &y0) == ESP_OK &&
                    nvs_get_i32(h, "x1", &x1) == ESP_OK && nvs_get_i32(h, "y1", &y1) == ESP_OK &&
                    nvs_get_i32(h, "sx0", &sx0) == ESP_OK && nvs_get_i32(h, "sy0", &sy0) == ESP_OK &&
                    nvs_get_i32(h, "sx1", &sx1) == ESP_OK && nvs_get_i32(h, "sy1", &sy1) == ESP_OK &&
                    nvs_get_i32(h, "swap", &swap) == ESP_OK;
    nvs_close(h);
    if (!ok) {
        return false;
    }
    out->x0 = x0;
    out->y0 = y0;
    out->x1 = x1;
    out->y1 = y1;
    out->sx0 = sx0;
    out->sy0 = sy0;
    out->sx1 = sx1;
    out->sy1 = sy1;
    out->swapXy = swap != 0;
    out->valid = true;
    return true;
}

bool TouchCalib::save(const TouchCalData& in) {
    if (!ensureNvs()) {
        return false;
    }
    nvs_handle_t h = 0;
    if (nvs_open(kNvsNs, NVS_READWRITE, &h) != ESP_OK) {
        return false;
    }
    nvs_set_i32(h, "magic", 0x54435033);  // 'TCP3'
    nvs_set_i32(h, "x0", in.x0);
    nvs_set_i32(h, "y0", in.y0);
    nvs_set_i32(h, "x1", in.x1);
    nvs_set_i32(h, "y1", in.y1);
    nvs_set_i32(h, "sx0", in.sx0);
    nvs_set_i32(h, "sy0", in.sy0);
    nvs_set_i32(h, "sx1", in.sx1);
    nvs_set_i32(h, "sy1", in.sy1);
    nvs_set_i32(h, "swap", in.swapXy ? 1 : 0);
    const esp_err_t err = nvs_commit(h);
    nvs_close(h);
    return err == ESP_OK;
}

void TouchCalib::apply(const TouchCalData& cal) {
    if (EspIdfDisplay::g_active) {
        EspIdfDisplay::g_active->setCal(cal);
    }
}

bool TouchCalib::run(Renderer& renderer, Display& display) {
    auto* d = EspIdfDisplay::g_active;
    if (!d) {
        Log::error("TouchCal", "no active display");
        return false;
    }

    TouchCalData cal = d->cal();
    const int w = display.width();
    const int h = display.height();
    // Resistive corners are dead — keep targets well inboard.
    const int m = std::max(40, std::min(w, h) / 5);
    const uint16_t bg = ZynColor::pack(0, 0, 0);
    const uint16_t fg = ZynColor::pack(255, 180, 40);
    const uint16_t dim = ZynColor::pack(120, 120, 120);

    struct Target {
        int sx;
        int sy;
        const char* label;
    };
    const Target targets[2] = {{m, m, "Touch TOP-LEFT"}, {w - 1 - m, h - 1 - m, "Touch BOTTOM-RIGHT"}};
    int raw[2][2] = {};

    for (int i = 0; i < 2; ++i) {
        Log::info("TouchCal", "%s", targets[i].label);
        drawCalFrame(renderer, targets[i].label, targets[i].sx, targets[i].sy, fg, dim, bg);
        if (!waitTouchSample(d, &raw[i][0], &raw[i][1])) {
            return false;
        }
        drawCalFrame(renderer, "OK — lift finger", targets[i].sx, targets[i].sy, fg, dim, bg);
        vTaskDelay(pdMS_TO_TICKS(300));
    }

    const int drx = std::abs(raw[1][0] - raw[0][0]);
    const int dry = std::abs(raw[1][1] - raw[0][1]);
    const bool screenXDom = (targets[1].sx - targets[0].sx) >= (targets[1].sy - targets[0].sy);
    const bool rawXDom = drx >= dry;
    cal.swapXy = screenXDom != rawXDom;
    auto pick = [&](int rx, int ry, bool wantX) {
        return cal.swapXy ? (wantX ? ry : rx) : (wantX ? rx : ry);
    };
    cal.x0 = pick(raw[0][0], raw[0][1], true);
    cal.y0 = pick(raw[0][0], raw[0][1], false);
    cal.x1 = pick(raw[1][0], raw[1][1], true);
    cal.y1 = pick(raw[1][0], raw[1][1], false);
    cal.sx0 = targets[0].sx;
    cal.sy0 = targets[0].sy;
    cal.sx1 = targets[1].sx;
    cal.sy1 = targets[1].sy;
    cal.valid = true;

    if (!save(cal)) {
        Log::error("TouchCal", "NVS save failed");
        return false;
    }
    apply(cal);
    Log::info("TouchCal", "saved x0=%d y0=%d x1=%d y1=%d swap=%d @(%d,%d)-(%d,%d)", cal.x0, cal.y0,
              cal.x1, cal.y1, cal.swapXy ? 1 : 0, cal.sx0, cal.sy0, cal.sx1, cal.sy1);

    renderer.beginFrame();
    renderer.clear(bg);
    renderer.drawText(8, 8, "CALIBRATION OK", fg, 2);
    renderer.drawText(8, 32, "Starting...", dim, 1);
    renderer.endFrame();
    vTaskDelay(pdMS_TO_TICKS(500));
    return true;
}

}  // namespace zyngine::hal
