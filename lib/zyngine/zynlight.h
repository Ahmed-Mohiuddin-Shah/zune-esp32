#ifndef ZYNLIGHT_H
#define ZYNLIGHT_H

#include <zynmath.h>

struct ZDirectionalLight {
    float intensity;
    ZVec3 direction;

    ZVec3 rotation;
    ZVec3 directionVS;

    ZDirectionalLight(float intensity, ZVec3 direction)
        : intensity(intensity), direction(direction), rotation(ZVec3(0, 0, 0)), directionVS(ZVec3()) {}
};

#endif