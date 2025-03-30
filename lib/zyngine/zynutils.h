#ifndef ZYNUTILS_H
#define ZYNUTILS_H

#include <cstdint>
#include <raylib/raylib.h>

inline Color getRaylibColorFromRGB565(uint16_t color)
{
    return {(color >> 11) * 255 / 31, (color >> 5 & 0x3F) * 255 / 63, (color & 0x1F) * 255 / 31, 255};
}

inline uint16_t getIntensityRGB565(float intensity, uint16_t color) {
    // Extract RGB components from the 16-bit RGB565 color
    uint8_t r = (color >> 11) & 0x1F;
    uint8_t g = (color >> 5) & 0x3F;
    uint8_t b = color & 0x1F;

    // Apply intensity scaling
    r = static_cast<uint8_t>(r * intensity);
    g = static_cast<uint8_t>(g * intensity);
    b = static_cast<uint8_t>(b * intensity);

    // Clamp values to their respective ranges
    r = r > 31 ? 31 : r;
    g = g > 63 ? 63 : g;
    b = b > 31 ? 31 : b;

    // Reconstruct the RGB565 color
    return (r << 11) | (g << 5) | b;
}

#endif