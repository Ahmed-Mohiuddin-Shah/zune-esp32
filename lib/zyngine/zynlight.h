#ifndef ZYNLIGHT_H
#define ZYNLIGHT_H

#include <zynmath.h>

struct ZynLight
{
    ZVec3 l;

    ZynLight(ZVec3 v = ZVec3(1.0f, -1.0f, 1.0f)) : l(v)
    {
        l = l.normalized();
    }

    float getIntensityAtTriangle(ZTriangle tri)
    {
        ZVec3 n = (tri.v[2].sub(tri.v[0])).cross(tri.v[1].sub(tri.v[0]));
        return l.dot(n.normalized());
    }

    float getIntensityAtNorm(ZVec3 n)
    {
        return std::max(0.0f, l.dot(n.normalized()));
    }
};

#endif