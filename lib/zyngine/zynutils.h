#ifndef ZYNUTILS_H
#define ZYNUTILS_H

#include <cstdint>
#include <zynmath.h>
#include <raylib/raylib.h>

inline Color getRaylibColorFromRGB565(uint16_t color)
{
    return {(color >> 11) * 255 / 31, (color >> 5 & 0x3F) * 255 / 63, (color & 0x1F) * 255 / 31, 255};
}

// TODO Revise function
inline uint16_t getRGB565FromZVec3(ZVec3 c)
{

    uint16_t rgb565 = (uint16_t)c.x | (uint16_t)c.y | (uint16_t)c.z;

    return rgb565;
}

#endif