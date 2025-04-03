#ifndef ZYNUTILS_H
#define ZYNUTILS_H

#include <cstdint>
#include <cstdio>
#include <raylib/raylib.h>
#include <zynmath.h>

inline Color getRaylibColorFromRGB565(uint16_t color)
{
    return {static_cast<unsigned char>((color >> 11) * 255 / 31),
            static_cast<unsigned char>((color >> 5 & 0x3F) * 255 / 63),
            static_cast<unsigned char>((color & 0x1F) * 255 / 31),
            255};
}

inline uint16_t getIntensityRGB565(float intensity, uint16_t color)
{
    // printf("%f\n", intensity);
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

inline bool isInClipView(ZVec3 *points, float nearPlane, float farPlane)
{
    float z = 0;
    for (int i = 0; i < 3; i++)
    {
        z += points[i].z;
    }

    z = z / 3;

    return z < nearPlane && z > farPlane;
}

#endif