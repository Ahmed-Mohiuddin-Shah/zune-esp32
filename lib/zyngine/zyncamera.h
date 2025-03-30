#ifndef ZYNCAMERA_H
#define ZYNCAMERA_H

#include <zynmath.h>

struct ZynCamera
{
    ZVec3 c;

    ZynCamera(float x = 0.0f, float y = 0.0f, float z = 3.0f) : c(ZVec3(x, y, z)) {}
};

#endif