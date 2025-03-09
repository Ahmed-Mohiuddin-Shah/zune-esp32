#include <Arduino.h>

#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <ps2.h>

#include "Audio.h"
// I2S Connections for Speaker
#define I2S_DOUT 40
#define I2S_BCLK 41
#define I2S_LRC 42

class LGFX : public lgfx::LGFX_Device
{
    lgfx::Panel_ILI9486 _panel_instance;
    lgfx::Bus_Parallel8 _bus_instance; // 8位并行总线实例（仅限ESP32）

public:
    LGFX(void)
    {
        {                                      // 设置总线控制。
            auto cfg = _bus_instance.config(); // 获取用于总线设置的结构。

            // 8位并行总线设置
            cfg.freq_write = 25000000; // 发送时钟（最大可将20MHz、80MHz舍入为整数）
            cfg.pin_wr = 12;           // 连接WR的针脚编号
            cfg.pin_rd = 46;           // 连接RD的针脚编号
            cfg.pin_rs = 13;           // 连接RS（D/C）的针脚编号

            cfg.pin_d0 = 4;  // 连接D0的管脚编号
            cfg.pin_d1 = 5;  // 连接D1的管脚编号
            cfg.pin_d2 = 6;  // 连接D2的管脚编号
            cfg.pin_d3 = 7;  // 连接D3的管脚编号
            cfg.pin_d4 = 15; // 连接D4的管脚编号
            cfg.pin_d5 = 16; // 连接D5的管脚编号
            cfg.pin_d6 = 10; // 连接D6的管脚编号
            cfg.pin_d7 = 11; // 连接D7的管脚编号

            _bus_instance.config(cfg);              // 在总线上反映设置值。
            _panel_instance.setBus(&_bus_instance); // 把总线设置在面板上。
        }

        {                                        // 设置显示面板控制。
            auto cfg = _panel_instance.config(); // 获取用于设置显示面板的结构。
            cfg.pin_cs = 14;                     // 连接CS的管脚编号（-1=disable）
            cfg.pin_rst = -1;                    // 连接RST的管脚编号（-1=disable）
            cfg.pin_busy = -1;                   // 连接BUSY的管脚编号（-1=disable）
            cfg.memory_width = 320;              // 驱动器IC支持的最大宽度
            cfg.memory_height = 480;             // 驱动器IC支持的最大高度
            cfg.panel_width = 320;               // 实际可见宽度
            cfg.panel_height = 480;              // 实际可见高度
            cfg.offset_x = 0;                    // 面板X方向偏移量
            cfg.offset_y = 0;                    // 面板Y方向偏移量
            cfg.offset_rotation = 0;             // 旋转方向值的偏移0~7（4~7上下反转）
            cfg.dummy_read_pixel = 8;            // 像素读取前虚拟读位数
            cfg.dummy_read_bits = 1;             // 读取非像素数据之前的虚拟读位数
            cfg.readable = true;                 // 可读取数据时设置为真
            cfg.invert = false;                  // 如果面板的明暗颠倒，则设置为真
            cfg.rgb_order = false;               // 如果面板中的红色和蓝色替换为真
            cfg.dlen_16bit = false;              // 在以16 bit为单位发送数据长度的面板的情况下设定为真
            cfg.bus_shared = true;               // 与SD卡共享总线时设置为真（通过drawJpgFile等进行总线控制）

            _panel_instance.config(cfg);
        }

        setPanel(&_panel_instance); // 设置要使用的面板。
    }
};

static LGFX lcd;
static LGFX_Sprite _sprites[2];

struct ball_info_t
{
    int32_t x;
    int32_t y;
    int32_t dx;
    int32_t dy;
    int32_t r;
    int32_t m;
    uint32_t color;
};

PS2 mouse(45, 38);

static constexpr std::uint32_t SHIFTSIZE = 8;
static constexpr std::uint32_t BALL_MAX = 100;

static ball_info_t _balls[2][BALL_MAX];
static std::uint32_t _ball_count = 0, _fps = 0;
static std::uint32_t ball_count = 0;
static std::uint32_t sec, psec;
static std::uint32_t fps = 0, frame_count = 0;

static std::uint32_t _width;
static std::uint32_t _height;

volatile bool _is_running;
volatile std::uint32_t _draw_count;
volatile std::uint32_t _loop_count;

char mstat, mx, my;
int m, x, y;

void MouseRead()
{
    /* Get a reading from the mouse */
    mouse.write(0xeb); // give me data!
    mouse.read();      // ignore ack
    mstat = mouse.read();
    mx = mouse.read();
    my = mouse.read();

    /* Convert the data to useable form(int)*/

    m = (int)mstat;
    x = (int)mx;
    y = (int)my;
}

static void diffDraw(LGFX_Sprite *sp0, LGFX_Sprite *sp1)
{
    union
    {
        std::uint32_t *s32;
        std::uint8_t *s;
    };
    union
    {
        std::uint32_t *p32;
        std::uint8_t *p;
    };
    s32 = (std::uint32_t *)sp0->getBuffer();
    p32 = (std::uint32_t *)sp1->getBuffer();

    auto width = sp0->width();
    auto height = sp0->height();

    auto w32 = (width + 3) >> 2;
    std::int32_t y = 0;
    do
    {
        std::int32_t x32 = 0;
        do
        {
            while (s32[x32] == p32[x32] && ++x32 < w32)
                ;
            if (x32 == w32)
                break;

            std::int32_t xs = x32 << 2;
            while (s[xs] == p[xs])
                ++xs;

            while (++x32 < w32 && s32[x32] != p32[x32])
                ;

            std::int32_t xe = (x32 << 2) - 1;
            if (xe >= width)
                xe = width - 1;
            while (s[xe] == p[xe])
                --xe;

            lcd.pushImage(xs, y, xe - xs + 1, 1, &s[xs]);
        } while (x32 < w32);
        s32 += w32;
        p32 += w32;
    } while (++y < height);
    lcd.display();
}

static void drawfunc(void)
{
    ball_info_t *balls;
    ball_info_t *a;
    LGFX_Sprite *sprite;

    auto width = _sprites[0].width();
    auto height = _sprites[0].height();

    std::size_t flip = _draw_count & 1;
    balls = &_balls[flip][0];

    sprite = &(_sprites[flip]);
    sprite->clear();

    for (int32_t i = 8; i < width; i += 16)
    {
        sprite->drawFastVLine(i, 0, height, 0x1F);
    }
    for (int32_t i = 8; i < height; i += 16)
    {
        sprite->drawFastHLine(0, i, width, 0x1F);
    }
    for (std::uint32_t i = 0; i < _ball_count; i++)
    {
        a = &balls[i];
        sprite->fillCircle(a->x >> SHIFTSIZE, a->y >> SHIFTSIZE, a->r >> SHIFTSIZE, a->color);
        // sprite->fillRect((a->x - a->r) >> SHIFTSIZE
        //                   , (a->y - a->r) >> SHIFTSIZE
        //                   , (a->r * 2) >> SHIFTSIZE
        //                   , (a->r * 2) >> SHIFTSIZE
        //                   , a->color);
    }

    sprite->setCursor(1, 1);
    sprite->setTextColor(TFT_BLACK);
    sprite->printf("obj:%d fps:%d", _ball_count, _fps);
    sprite->setCursor(0, 0);
    sprite->setTextColor(TFT_WHITE);
    sprite->printf("obj:%d fps:%d", _ball_count, _fps);
    sprite->setCursor(20, 20);
    sprite->printf("x: %d, y: %d", x, y);
    diffDraw(&_sprites[flip], &_sprites[!flip]);
    ++_draw_count;
}

static void mainfunc(void)
{
    static constexpr float e = 0.999; // Coefficient of friction

    sec = lgfx::millis() / 500;
    if (psec != sec)
    {
        psec = sec;
        fps = frame_count * 2;
        frame_count = 0;

        if (++ball_count >= BALL_MAX)
        {
            ball_count = 1;
        }
        auto a = &_balls[_loop_count & 1][ball_count - 1];
        a->color = lgfx::color565(100 + (rand() % 155), 100 + (rand() % 155), 100 + (rand() % 155));
        a->x = 0;
        a->y = 0;
        a->dx = (rand() & (3 << SHIFTSIZE)) + 1;
        a->dy = (rand() & (3 << SHIFTSIZE)) + 1;
        a->r = (4 + (ball_count & 0x07)) << SHIFTSIZE;
        a->m = 4 + (ball_count & 0x07);
#if defined(ESP32) || defined(CONFIG_IDF_TARGET_ESP32) || defined(ESP_PLATFORM)
        vTaskDelay(1);
#endif
    }

    frame_count++;
    _loop_count++;

    ball_info_t *a, *b, *balls;
    int32_t rr, len, vx2vy2;
    float vx, vy, distance, t;

    size_t f = _loop_count & 1;
    balls = a = &_balls[f][0];
    b = &_balls[!f][0];
    memcpy(a, b, sizeof(ball_info_t) * ball_count);

    for (int i = 0; i != ball_count; i++)
    {
        a = &balls[i];
        //  a->dy += 4; // gravity

        a->x += a->dx;
        if (a->x < a->r)
        {
            a->x = a->r;
            if (a->dx < 0)
                a->dx = -a->dx * e;
        }
        else if (a->x >= _width - a->r)
        {
            a->x = _width - a->r - 1;
            if (a->dx > 0)
                a->dx = -a->dx * e;
        }
        a->y += a->dy;
        if (a->y < a->r)
        {
            a->y = a->r;
            if (a->dy < 0)
                a->dy = -a->dy * e;
        }
        else if (a->y >= _height - a->r)
        {
            a->y = _height - a->r - 1;
            if (a->dy > 0)
                a->dy = -a->dy * e;
        }
        for (int j = i + 1; j != ball_count; j++)
        {
            b = &balls[j];

            rr = a->r + b->r;
            vx = a->x - b->x;
            if (abs(vx) > rr)
                continue;
            vy = a->y - b->y;
            if (abs(vy) > rr)
                continue;

            len = sqrt(vx * vx + vy * vy);
            if (len >= rr)
                continue;
            if (len == 0.0)
                continue;
            distance = (rr - len) >> 1;
            vx *= distance / len;
            vy *= distance / len;

            a->x += vx;
            b->x -= vx;
            vx = b->x - a->x;

            a->y += vy;
            b->y -= vy;
            vy = b->y - a->y;

            vx2vy2 = vx * vx + vy * vy;

            t = -(vx * a->dx + vy * a->dy) / vx2vy2;
            float arx = a->dx + vx * t;
            float ary = a->dy + vy * t;

            t = -(-vy * a->dx + vx * a->dy) / vx2vy2;
            float amx = a->dx - vy * t;
            float amy = a->dy + vx * t;

            t = -(vx * b->dx + vy * b->dy) / vx2vy2;
            float brx = b->dx + vx * t;
            float bry = b->dy + vy * t;

            t = -(-vy * b->dx + vx * b->dy) / vx2vy2;
            float bmx = b->dx - vy * t;
            float bmy = b->dy + vx * t;

            float adx = (a->m * amx + b->m * bmx + bmx * e * b->m - amx * e * b->m) / (a->m + b->m);
            float bdx = -e * (bmx - amx) + adx;
            float ady = (a->m * amy + b->m * bmy + bmy * e * b->m - amy * e * b->m) / (a->m + b->m);
            float bdy = -e * (bmy - amy) + ady;

            a->dx = roundf(adx + arx);
            a->dy = roundf(ady + ary);
            b->dx = roundf(bdx + brx);
            b->dy = roundf(bdy + bry);
        }
    }

    _fps = fps;
    _ball_count = ball_count;
}

#if defined(ESP32) || defined(CONFIG_IDF_TARGET_ESP32) || defined(ESP_PLATFORM)
static void taskDraw(void *)
{
    while (_is_running)
    {
        while (_loop_count == _draw_count)
        {
            taskYIELD();
        }
        drawfunc();
    }
    vTaskDelete(NULL);
}
#endif

Audio audio;

void setup()
{

    // Setup I2S
    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);

    // Set Volume
    audio.setVolume(5);
    /* Mouse initialization */
    mouse.write(0xff); // reset
    mouse.read();      // ack byte
    mouse.read();      // blank */
    mouse.read();      // blank */
    mouse.write(0xf0); // remote mode
    mouse.read();      // ack
    delayMicroseconds(100);
    pinMode(2, OUTPUT);
    digitalWrite(2, HIGH);
    lcd.begin();
    lcd.startWrite();
    lcd.setColorDepth(16);
    if (lcd.width() < lcd.height())
        lcd.setRotation(lcd.getRotation() ^ 1);

    auto lcd_width = lcd.width();
    auto lcd_height = lcd.height();

    for (std::uint32_t i = 0; i < 2; ++i)
    {
        _sprites[i].setTextSize(2);
        _sprites[i].setColorDepth(8);
    }

    bool fail = false;
    for (std::uint32_t i = 0; !fail && i < 2; ++i)
    {
        fail = !_sprites[i].createSprite(lcd_width, lcd_height);
    }

    if (fail)
    {
        fail = false;
        for (std::uint32_t i = 0; !fail && i < 2; ++i)
        {
            _sprites[i].setPsram(true);
            fail = !_sprites[i].createSprite(lcd_width, lcd_height);
        }

        if (fail)
        {
            fail = false;
            if (lcd_width > 320)
                lcd_width = 320;
            if (lcd_height > 240)
                lcd_height = 240;

            for (std::uint32_t i = 0; !fail && i < 2; ++i)
            {
                _sprites[i].setPsram(true);
                fail = !_sprites[i].createSprite(lcd_width, lcd_height);
            }
            if (fail)
            {
                lcd.print("createSprite fail...");
                lgfx::delay(3000);
            }
        }
    }

    _width = lcd_width << SHIFTSIZE;
    _height = lcd_height << SHIFTSIZE;

    for (std::uint32_t i = 0; i < ball_count; ++i)
    {
        auto a = &_balls[_loop_count & 1][i];
        a->color = lgfx::color565(100 + (rand() % 155), 100 + (rand() % 155), 100 + (rand() % 155));
        a->x = 0;
        a->y = 0;
        a->dx = (rand() & (3 << SHIFTSIZE)) + 1;
        a->dy = (rand() & (3 << SHIFTSIZE)) + 1;
        a->r = (4 + (i & 0x07)) << SHIFTSIZE;
        a->m = 4 + (i & 0x07);
    }

    _is_running = true;
    _draw_count = 0;
    _loop_count = 0;

#if defined(CONFIG_IDF_TARGET_ESP32)
    xTaskCreate(taskDraw, "taskDraw", 2048, NULL, 0, NULL);
#endif
}

void loop(void)
{
    mainfunc();
#if defined(CONFIG_IDF_TARGET_ESP32)
    while (_loop_count != _draw_count)
    {
        taskYIELD();
    }
#else
    MouseRead();

    drawfunc();
#endif
}