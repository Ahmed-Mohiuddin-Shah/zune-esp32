#include "zyngine/hal/display.hpp"
#include "zyngine/hal/gpio.hpp"
#include "zyngine/hal/input.hpp"
#include "zyngine/hal/log.hpp"

#include "esp32_s3_zune/board.hpp"

#include <LovyanGFX.hpp>

#include <cstring>
#include <memory>
#include <vector>

namespace zyngine::hal {
namespace {

class ParallelILI9486 : public lgfx::LGFX_Device {
public:
    ParallelILI9486() {
        const auto board = zyngine::board::esp32S3ZuneBoard();
        {
            auto cfg = _bus.config();
            cfg.freq_write = 30000000;
            cfg.pin_wr = board.pins.lcdWr;
            cfg.pin_rd = board.pins.lcdRd;
            cfg.pin_rs = board.pins.lcdRs;
            cfg.pin_d0 = board.pins.lcdD0;
            cfg.pin_d1 = board.pins.lcdD1;
            cfg.pin_d2 = board.pins.lcdD2;
            cfg.pin_d3 = board.pins.lcdD3;
            cfg.pin_d4 = board.pins.lcdD4;
            cfg.pin_d5 = board.pins.lcdD5;
            cfg.pin_d6 = board.pins.lcdD6;
            cfg.pin_d7 = board.pins.lcdD7;
            _bus.config(cfg);
            _panel.setBus(&_bus);
        }
        {
            auto cfg = _panel.config();
            cfg.pin_cs = board.pins.lcdCs;
            cfg.pin_rst = -1;
            cfg.pin_busy = -1;
            cfg.memory_width = 320;
            cfg.memory_height = 480;
            cfg.panel_width = 320;
            cfg.panel_height = 480;
            cfg.offset_x = 0;
            cfg.offset_y = 0;
            cfg.offset_rotation = 0;
            cfg.dummy_read_pixel = 8;
            cfg.dummy_read_bits = 1;
            cfg.readable = true;
            cfg.invert = false;
            cfg.rgb_order = false;
            cfg.dlen_16bit = false;
            cfg.bus_shared = true;
            _panel.config(cfg);
        }
        setPanel(&_panel);
    }

private:
    lgfx::Panel_ILI9486 _panel;
    lgfx::Bus_Parallel8 _bus;
};

class EspIdfDisplay final : public Display {
public:
    bool begin(int width, int height) override {
        width_ = width;
        height_ = height;
        const auto board = zyngine::board::esp32S3ZuneBoard();
        lcd_ = std::make_unique<ParallelILI9486>();
        lcd_->begin();
        lcd_->setColorDepth(16);
        lcd_->setRotation(board.display.rotation);
        if (board.pins.backlight >= 0) {
            Gpio::setMode(board.pins.backlight, PinMode::Output);
            Gpio::write(board.pins.backlight, PinLevel::High);
        }
        previous_.assign(static_cast<size_t>(width_ * height_), 0);
        Log::info("Display", "ESP-IDF LovyanGFX ILI9486 %dx%d", width_, height_);
        return true;
    }

    void end() override { lcd_.reset(); }

    int width() const override { return width_; }
    int height() const override { return height_; }

    void present(const uint16_t* pixels, int width, int height) override {
        if (!lcd_) {
            return;
        }
        // Dirty-row present: push contiguous changed spans per scanline.
        for (int y = 0; y < height; ++y) {
            const uint16_t* row = pixels + y * width;
            uint16_t* prev = previous_.data() + y * width;
            int x = 0;
            while (x < width) {
                while (x < width && row[x] == prev[x]) {
                    ++x;
                }
                if (x >= width) {
                    break;
                }
                int start = x;
                while (x < width && row[x] != prev[x]) {
                    ++x;
                }
                lcd_->pushImage(start, y, x - start, 1, row + start);
            }
            std::memcpy(prev, row, static_cast<size_t>(width) * sizeof(uint16_t));
        }
        lcd_->display();
    }

    void setBacklight(bool on) override {
        const auto board = zyngine::board::esp32S3ZuneBoard();
        if (board.pins.backlight >= 0) {
            Gpio::write(board.pins.backlight, on ? PinLevel::High : PinLevel::Low);
        }
    }

    bool shouldContinue() const override { return true; }

private:
    int width_ = 0;
    int height_ = 0;
    std::unique_ptr<ParallelILI9486> lcd_;
    std::vector<uint16_t> previous_;
};

PointerState g_pointer{};

}  // namespace

Display* createDisplay() { return new EspIdfDisplay(); }

PointerState Input::pointer() { return g_pointer; }

bool Input::secondaryDown() { return false; }

void Input::scrollDelta(float* dx, float* dy) {
    if (dx) *dx = 0.f;
    if (dy) *dy = 0.f;
}

}  // namespace zyngine::hal
