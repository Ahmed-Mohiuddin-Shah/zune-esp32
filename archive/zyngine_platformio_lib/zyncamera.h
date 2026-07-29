#ifndef ZYNCAMERA_H
#define ZYNCAMERA_H

#include <zynmath.h>

struct ZynCamera
{
    ZVec3 center;
    ZVec3 eye;
    ZVec3 up;

    float fov_degrees = 60.0f;                        // Field of view (e.g., 60°)
    float aspect_ratio = (float)320.0 / (float)480.0; // Screen aspect ratio
    float near = 0.1f;                                // Near clipping plane
    float far = 100.0f;                               // Far clipping plane

    ZynCamera() : center(ZVec3(2.0f, 2.0f, -2.0f)), eye(ZVec3(1.0f, 1.0f, 3.0f)), up(ZVec3(0.0f, 1.0f, 0.0f)) {}

    ZMat4 getLookAtMatrix()
    {
        ZVec3 z = (eye.sub(center)).normalized(); // Forward axis
        ZVec3 x = (up.cross(z)).normalized();     // Right axis (cross product)
        ZVec3 y = (z.cross(x)).normalized();      // Corrected up axis

        ZMat4 Minv; // Inverse of camera transform
        ZMat4 Tr;   // Translation

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

        return Minv.mulMatrix(Tr); // View matrix = inverse camera transform
    }

    ZMat4 getProjectionMatrix()
    {
        ZMat4 projection;
        float fov_rad = fov_degrees * DEG2RAD;
        float tan_half_fov = tan(fov_rad / 2.0f);

        // Standard perspective matrix (similar to OpenGL)
        projection.m00 = 1.0f / (aspect_ratio * tan_half_fov); // X scaling
        projection.m11 = 1.0f / tan_half_fov;                  // Y scaling
        projection.m22 = -(far + near) / (far - near);         // Z depth
        projection.m23 = -2.0f * far * near / (far - near);    // Z offset
        projection.m32 = -1.0f;                                // Homogeneous divide

        return projection;
    }

    ZynCamera copy()
    {
        ZynCamera cam;
        cam.eye = eye;
        cam.center = center;
        cam.up = up;

        return cam;
    }
};

#endif