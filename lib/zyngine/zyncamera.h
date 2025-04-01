#ifndef ZYNCAMERA_H
#define ZYNCAMERA_H

#include <zynmath.h>

struct ZynCamera
{
    ZVec3 position;
    ZVec3 eye;
    ZVec3 center;

    ZynCamera() : position(ZVec3(0.0f, 0.0f, 3.0f)), eye(ZVec3(1.0f, 1.0f, 3.0f)), center(ZVec3(0.0f, 0.0f, 0.0f)) {}
};

#endif