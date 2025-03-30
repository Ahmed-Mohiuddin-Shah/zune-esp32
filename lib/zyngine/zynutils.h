#ifndef ZYNUTILS_H
#define ZYNUTILS_H

#include <cstdint>
#include <raylib/raylib.h>

inline Color getRaylibColorFromRGB565(uint16_t color)
{
    return {(color >> 11) * 255 / 31, (color >> 5 & 0x3F) * 255 / 63, (color & 0x1F) * 255 / 31, 255};
}

#endif