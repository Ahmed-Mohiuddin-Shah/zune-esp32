#pragma once

#include <cstdint>

namespace zyngine {

// 16-bit RGB565 palette
constexpr uint16_t ZYN_WHITE = 0xFFFF;
constexpr uint16_t ZYN_LIGHTGRAY = 0xCE59;
constexpr uint16_t ZYN_GRAY = 0x8410;
constexpr uint16_t ZYN_DARKGRAY = 0x528A;
constexpr uint16_t ZYN_YELLOW = 0xFFC0;
constexpr uint16_t ZYN_GOLD = 0xFE40;
constexpr uint16_t ZYN_ORANGE = 0xFD00;
constexpr uint16_t ZYN_PINK = 0xFB78;
constexpr uint16_t ZYN_RED = 0xE146;
constexpr uint16_t ZYN_MAROON = 0xB906;
constexpr uint16_t ZYN_GREEN = 0x0726;
constexpr uint16_t ZYN_LIME = 0x04E5;
constexpr uint16_t ZYN_DARKGREEN = 0x03A5;
constexpr uint16_t ZYN_SKYBLUE = 0x65FF;
constexpr uint16_t ZYN_BLUE = 0x03DE;
constexpr uint16_t ZYN_DARKBLUE = 0x0295;
constexpr uint16_t ZYN_PURPLE = 0xCBDF;
constexpr uint16_t ZYN_VIOLET = 0x81F7;
constexpr uint16_t ZYN_DARKPURPLE = 0x70EF;
constexpr uint16_t ZYN_BEIGE = 0xD590;
constexpr uint16_t ZYN_BROWN = 0x7B49;
constexpr uint16_t ZYN_DARKBROWN = 0x49E5;
constexpr uint16_t ZYN_BLACK = 0x0000;
constexpr uint16_t ZYN_MAGENTA = 0xF81F;
constexpr uint16_t ZYN_RAYWHITE = 0xF7BE;

/// Named ZynColor to avoid colliding with Raylib's ::Color.
struct ZynColor {
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
    uint16_t rgb565 = 0;

    ZynColor() = default;

    ZynColor(uint8_t r_, uint8_t g_, uint8_t b_) : r(r_), g(g_), b(b_) {
        rgb565 = static_cast<uint16_t>(((r_ & 0xF8) << 8) | ((g_ & 0xFC) << 3) | (b_ >> 3));
    }

    explicit ZynColor(uint16_t c) : rgb565(c) {
        r = static_cast<uint8_t>((c >> 8) & 0xF8);
        g = static_cast<uint8_t>((c >> 3) & 0xFC);
        b = static_cast<uint8_t>((c << 3) & 0xF8);
    }

    static ZynColor fromRgb565(uint16_t c) { return ZynColor(c); }

    static uint16_t pack(uint8_t r, uint8_t g, uint8_t b) {
        return static_cast<uint16_t>(((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3));
    }

    static void unpack(uint16_t c, uint8_t& r, uint8_t& g, uint8_t& b) {
        r = static_cast<uint8_t>(((c >> 11) & 0x1F) * 255 / 31);
        g = static_cast<uint8_t>(((c >> 5) & 0x3F) * 255 / 63);
        b = static_cast<uint8_t>((c & 0x1F) * 255 / 31);
    }
};

inline uint16_t intensityRgb565(float intensity, uint16_t color) {
    uint8_t r = (color >> 11) & 0x1F;
    uint8_t g = (color >> 5) & 0x3F;
    uint8_t b = color & 0x1F;
    r = static_cast<uint8_t>(r * intensity);
    g = static_cast<uint8_t>(g * intensity);
    b = static_cast<uint8_t>(b * intensity);
    if (r > 31) r = 31;
    if (g > 63) g = 63;
    if (b > 31) b = 31;
    return static_cast<uint16_t>((r << 11) | (g << 5) | b);
}

using zyncolor = ZynColor;
inline uint16_t getIntensityRGB565(float intensity, uint16_t color) {
    return intensityRgb565(intensity, color);
}

}  // namespace zyngine

#ifndef ZYN_COLOR_LEGACY_MACROS
#define ZYN_COLOR_LEGACY_MACROS
#define ZYN_WHITE ::zyngine::ZYN_WHITE
#define ZYN_LIGHTGRAY ::zyngine::ZYN_LIGHTGRAY
#define ZYN_GRAY ::zyngine::ZYN_GRAY
#define ZYN_DARKGRAY ::zyngine::ZYN_DARKGRAY
#define ZYN_YELLOW ::zyngine::ZYN_YELLOW
#define ZYN_GOLD ::zyngine::ZYN_GOLD
#define ZYN_ORANGE ::zyngine::ZYN_ORANGE
#define ZYN_PINK ::zyngine::ZYN_PINK
#define ZYN_RED ::zyngine::ZYN_RED
#define ZYN_MAROON ::zyngine::ZYN_MAROON
#define ZYN_GREEN ::zyngine::ZYN_GREEN
#define ZYN_LIME ::zyngine::ZYN_LIME
#define ZYN_DARKGREEN ::zyngine::ZYN_DARKGREEN
#define ZYN_SKYBLUE ::zyngine::ZYN_SKYBLUE
#define ZYN_BLUE ::zyngine::ZYN_BLUE
#define ZYN_DARKBLUE ::zyngine::ZYN_DARKBLUE
#define ZYN_PURPLE ::zyngine::ZYN_PURPLE
#define ZYN_VIOLET ::zyngine::ZYN_VIOLET
#define ZYN_DARKPURPLE ::zyngine::ZYN_DARKPURPLE
#define ZYN_BEIGE ::zyngine::ZYN_BEIGE
#define ZYN_BROWN ::zyngine::ZYN_BROWN
#define ZYN_DARKBROWN ::zyngine::ZYN_DARKBROWN
#define ZYN_BLACK ::zyngine::ZYN_BLACK
#define ZYN_MAGENTA ::zyngine::ZYN_MAGENTA
#define ZYN_RAYWHITE ::zyngine::ZYN_RAYWHITE
#endif
