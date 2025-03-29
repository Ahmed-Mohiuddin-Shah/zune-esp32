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

inline ZVec3 lerpVector2(ZVec3 a, ZVec3 b, float per)
{
    return a.mul(1 - per).add(b.mul(per));
}

inline ZVec2 lerpVector2(ZVec2 a, ZVec2 b, float per)
{
    return a.mul(1 - per).add(b.mul(per));
}

// Perspective-correct vertex attribute interpolation (Vec3, Vec3)
inline ZVec3 lerp2AttributeVec3(
    ZVec3 a, ZVec3 b,   // vertex attributes to interpolate
    float w0, float w1, // barycentric weights for a and b, respectively
    float z0, float z1, // z values of a and b, respectively
    float z             // z value of the current fragment
)
{
    ZVec3 wa = a.mul(w0 / z0 * z);
    ZVec3 wb = b.mul(w1 / z1 * z);

    return ZVec3(wa.x + wb.x, wa.y + wb.y, wa.z + wb.z);
}

#endif