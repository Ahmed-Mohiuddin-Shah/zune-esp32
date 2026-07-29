#pragma once

#include "zyngine/graphics/math.hpp"

struct ZynCamera {
    ZVec3 center;
    ZVec3 eye;
    ZVec3 up;

    float fov_degrees = 60.0f;
    float aspect_ratio = 320.0f / 480.0f;
    float near = 0.1f;
    float far = 100.0f;

    ZynCamera()
        : center(ZVec3(2.0f, 2.0f, -2.0f)),
          eye(ZVec3(1.0f, 1.0f, 3.0f)),
          up(ZVec3(0.0f, 1.0f, 0.0f)) {}

    ZMat4 getLookAtMatrix() {
        ZVec3 z = (eye.sub(center)).normalized();
        ZVec3 x = (up.cross(z)).normalized();
        ZVec3 y = (z.cross(x)).normalized();

        ZMat4 Minv;
        ZMat4 Tr;

        Minv.m00 = x.x;
        Minv.m10 = y.x;
        Minv.m20 = z.x;
        Minv.m01 = x.y;
        Minv.m11 = y.y;
        Minv.m21 = z.y;
        Minv.m02 = x.z;
        Minv.m12 = y.z;
        Minv.m22 = z.z;

        Tr.m03 = -eye.x;
        Tr.m13 = -eye.y;
        Tr.m23 = -eye.z;

        return Minv.mulMatrix(Tr);
    }

    ZMat4 getProjectionMatrix() {
        ZMat4 projection;
        float fov_rad = fov_degrees * DEG2RAD;
        float tan_half_fov = tan(fov_rad / 2.0f);

        projection.m00 = 1.0f / (aspect_ratio * tan_half_fov);
        projection.m11 = 1.0f / tan_half_fov;
        projection.m22 = -(far + near) / (far - near);
        projection.m23 = -2.0f * far * near / (far - near);
        projection.m32 = -1.0f;

        return projection;
    }

    ZynCamera copy() {
        ZynCamera cam;
        cam.eye = eye;
        cam.center = center;
        cam.up = up;
        return cam;
    }
};
