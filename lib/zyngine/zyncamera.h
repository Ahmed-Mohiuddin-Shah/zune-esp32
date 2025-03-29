#ifndef ZYNCAMERA_H
#define ZYNCAMERA_H

#include <zynmath.h>

struct ZynCamera {
    ZVec3 position;
    ZVec3 rotation;

    ZMat4 cameraTransform;
};

#endif