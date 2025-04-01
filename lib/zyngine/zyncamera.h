#ifndef ZYNCAMERA_H
#define ZYNCAMERA_H

#include <zynmath.h>

struct ZynCamera
{
    ZVec3 center;
    ZVec3 eye;
    ZVec3 up;

    ZynCamera() : center(ZVec3(2.0f, 2.0f, -2.0f)), eye(ZVec3(1.0f, 1.0f, 3.0f)), up(ZVec3(0.0f, 1.0f, 0.0f)) {}

    ZMat4 getLookAtMatrix()
    {
        ZMat4 m;
        ZVec3 z = (eye.sub(center)).normalized();
        ZVec3 x = (up.cross(z)).normalized();
        ZVec3 y = (z.cross(x)).normalized();

        m.m00 = x.x;
        m.m10 = y.x;
        m.m20 = z.x;
        m.m03 = -center.x;

        m.m01 = x.y;
        m.m11 = y.y;
        m.m21 = z.y;
        m.m13 = -center.y;

        m.m03 = x.z;
        m.m13 = y.z;
        m.m23 = z.z;
        m.m33 = -center.z;

        return m;
    }
};

#endif