#ifndef ZYNLIGHT_H
#define ZYNLIGHT_H

#include <zynmath.h>

struct ZynLight
{
    ZVec3 l;

    ZynLight(ZVec3 v = ZVec3(0.0f, 0.0f, -1.0f)) : l(v) {}

    float getIntensityAtTriangle(ZTriangle tri)
    {
        ZVec3 n = (tri.v[2].sub(tri.v[0])).cross(tri.v[1].sub(tri.v[0]));

        n.normalize();
        return l.dot(n);
    }
};

#endif