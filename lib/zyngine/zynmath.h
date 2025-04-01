#ifndef ZYNMATH_H
#define ZYNMATH_H

#include <config_user.h>
#include <math.h>

struct ZVec2i
{
    int x, y;

    ZVec2i(int x = 0, int y = 0) : x(x), y(y) {}
};

struct ZVec2
{
    float x, y;
    ZVec2(float x = 0, float y = 0)
        : x(x), y(y) {}

    float normalize()
    {
        float length = getLength();
        float invLength = 1.0f / length;

        x *= invLength;
        y *= invLength;

        return length;
    }

    ZVec2 normalized()
    {
        float invLength = 1.0f / getLength();

        return ZVec2(x * invLength, y * invLength);
    }

    float getLength()
    {
        return sqrt(x * x + y * y);
    }

    float dot(ZVec2 v)
    {
        return x * v.x + y * v.y;
    }

    float cross(ZVec2 v)
    {
        return y * v.x - x * v.y;
    }

    ZVec2 add(ZVec2 v)
    {
        return ZVec2(x + v.x, y + v.y);
    }

    ZVec2 sub(ZVec2 v)
    {
        return ZVec2(x - v.x, y - v.y);
    }

    // TODO Check Implementation again
    ZVec2 div(ZVec2 v)
    {
        return ZVec2(x / v.x, y / v.y);
    }

    // TODO Check Implementation again
    ZVec2 mul(ZVec2 v)
    {
        return ZVec2(x * v.x, y * v.y);
    }

    ZVec2 mul(float f)
    {
        return ZVec2(x * f, y * f);
    }

    bool equals(ZVec2 v)
    {
        return x == v.x && y == v.y;
    }

    ZVec2i toVec2i()
    {
        return ZVec2i(x, y);
    }
};

struct ZVec3i
{
    int x, y, z;
    ZVec3i(int x = 0, int y = 0, int z = 0)
        : x(x), y(y), z(z) {}
};
struct ZVec3
{
    float x, y, z;
    ZVec3(float x = 0, float y = 0, float z = 0)
        : x(x), y(y), z(z) {}

    float normalize()
    {
        float length = getLength();
        float invLength = 1.0f / length;

        x *= invLength;
        y *= invLength;
        z *= invLength;

        return length;
    }

    ZVec3 normalized()
    {
        float invLength = 1.0f / getLength();

        return ZVec3(x * invLength, y * invLength, z * invLength);
    }

    float getLength()
    {
        return sqrt(x * x + y * y + z * z);
    }

    float dot(ZVec3 v)
    {
        return x * v.x + y * v.y + z * v.z;
    }

    ZVec3 cross(ZVec3 v)
    {
        return ZVec3(
            y * v.z - z * v.y,
            z * v.x - x * v.z,
            x * v.y - y * v.x);
    }

    ZVec3 add(ZVec3 v)
    {
        return ZVec3(x + v.x, y + v.y, z + v.z);
    }

    ZVec3 sub(ZVec3 v)
    {
        return ZVec3(x - v.x, y - v.y, z - v.z);
    }

    // TODO Verify Function Definition
    ZVec3 div(ZVec3 v)
    {
        return ZVec3(x / v.x, y / v.y, z / v.z);
    }

    ZVec3 divXYZ(float x, float y, float z)
    {
        return ZVec3(x / x, y / y, z / z);
    }

    // TODO Verify Function Definition
    ZVec3 mul(ZVec3 v)
    {
        return ZVec3(x * v.x, y * v.y, z * v.z);
    }

    ZVec3 mul(float f)
    {
        return ZVec3(x * f, y * f, z * f);
    }

    ZVec3 equals(ZVec3 v)
    {
        return x == v.x && y == v.y && z == v.z;
    }

    ZVec3i toZVec3i()
    {
        return ZVec3i(x, y, z);
    }
};

struct ZTriangle
{
    ZVec3 v[3];
    ZVec2 t[3];
};
struct ZVec4
{
    float x, y, z, w;
    ZVec4(float x = 0, float y = 0, float z = 0, float w = 0)
        : x(x), y(y), z(z), w(w) {}

    ZVec4(ZVec3 v) : x(v.x), y(v.y), z(v.z), w(1.0f) {}

    ZVec3 toZVec3()
    {
        return ZVec3(x / w, y / w, z / w);
    }

    ZVec3 toZVec3(ZVec4 v)
    {
        return ZVec3(v.x / v.w, v.y / v.w, v.z / v.w);
    }
};

typedef ZVec4 Quaternion;

struct ZMat4
{
    float m00, m01, m02, m03;
    float m10, m11, m12, m13;
    float m20, m21, m22, m23;
    float m30, m31, m32, m33;

    ZMat4()
    {
        // Identity matrix by default
        m00 = 1;
        m01 = 0;
        m02 = 0;
        m03 = 0;
        m10 = 0;
        m11 = 1;
        m12 = 0;
        m13 = 0;
        m20 = 0;
        m21 = 0;
        m22 = 1;
        m23 = 0;
        m30 = 0;
        m31 = 0;
        m32 = 0;
        m33 = 1;
    }

    void reset()
    {
        // Identity matrix by default
        m00 = 1;
        m01 = 0;
        m02 = 0;
        m03 = 0;
        m10 = 0;
        m11 = 1;
        m12 = 0;
        m13 = 0;
        m20 = 0;
        m21 = 0;
        m22 = 1;
        m23 = 0;
        m30 = 0;
        m31 = 0;
        m32 = 0;
        m33 = 1;
    }

    ZMat4 fromAxis(ZVec3 vx, ZVec3 vy, ZVec3 vz)
    {
        ZMat4 res;
        res.m00 = vx.x;
        res.m01 = vy.x;
        res.m02 = vz.x;
        res.m10 = vx.y;
        res.m11 = vy.y;
        res.m12 = vz.y;
        res.m20 = vx.z;
        res.m21 = vy.z;
        res.m22 = vz.z;

        return res;
    }

    ZMat4 mulMatrix(ZMat4 right)
    {
        ZMat4 res;
        res.m00 = m00 * right.m00 + m01 * right.m10 + m02 * right.m20 + m03 * right.m30;
        res.m01 = m00 * right.m01 + m01 * right.m11 + m02 * right.m21 + m03 * right.m31;
        res.m02 = m00 * right.m02 + m01 * right.m12 + m02 * right.m22 + m03 * right.m32;
        res.m03 = m00 * right.m03 + m01 * right.m13 + m02 * right.m23 + m03 * right.m33;

        res.m10 = m10 * right.m00 + m11 * right.m10 + m12 * right.m20 + m13 * right.m30;
        res.m11 = m10 * right.m01 + m11 * right.m11 + m12 * right.m21 + m13 * right.m31;
        res.m12 = m10 * right.m02 + m11 * right.m12 + m12 * right.m22 + m13 * right.m32;
        res.m13 = m10 * right.m03 + m11 * right.m13 + m12 * right.m23 + m13 * right.m33;

        res.m20 = m20 * right.m00 + m21 * right.m10 + m22 * right.m20 + m23 * right.m30;
        res.m21 = m20 * right.m01 + m21 * right.m11 + m22 * right.m21 + m23 * right.m31;
        res.m22 = m20 * right.m02 + m21 * right.m12 + m22 * right.m22 + m23 * right.m32;
        res.m23 = m20 * right.m03 + m21 * right.m13 + m22 * right.m23 + m23 * right.m33;

        res.m30 = m30 * right.m00 + m31 * right.m10 + m32 * right.m20 + m33 * right.m30;
        res.m31 = m30 * right.m01 + m31 * right.m11 + m32 * right.m21 + m33 * right.m31;
        res.m32 = m30 * right.m02 + m31 * right.m12 + m32 * right.m22 + m33 * right.m32;
        res.m33 = m30 * right.m03 + m31 * right.m13 + m32 * right.m23 + m33 * right.m33;

        return res;
    }

    ZVec3 mulVector(ZVec3 right, float w = 1.0f)
    {
        ZVec3 res = ZVec3(0.0f, 0.0f, 0.0f);

        res.x = m00 * right.x + m01 * right.y + m02 * right.z + m03 * w;
        res.y = m10 * right.x + m11 * right.y + m12 * right.z + m13 * w;
        res.z = m20 * right.x + m21 * right.y + m22 * right.z + m23 * w;

        return res;
    }

    ZVec4 mulVector(ZVec4 right)
    {
        ZVec4 res = ZVec4(0.0f, 0.0f, 0.0f, 1.0f);

        res.x = m00 * right.x + m01 * right.y + m02 * right.z + m03 * right.w;
        res.y = m10 * right.x + m11 * right.y + m12 * right.z + m13 * right.w;
        res.z = m20 * right.x + m21 * right.y + m22 * right.z + m23 * right.w;
        res.w = m30 * right.x + m31 * right.y + m32 * right.z + m33 * right.w;

        return res;
    }

    ZMat4 scale(float x, float y = -1, float z = -1)
    {
        if (y == -1 && z == -1)
        {
            y = x;
            z = x;
        }

        ZMat4 scale;
        scale.m00 = x;
        scale.m11 = y;
        scale.m22 = z;

        return this->mulMatrix(scale);
    }

    ZMat4 rotate(float x, float y, float z)
    {
        float sinX = sin(x);
        float cosX = cos(x);
        float sinY = sin(y);
        float cosY = cos(y);
        float sinZ = sin(z);
        float cosZ = cos(z);

        ZMat4 res;

        res.m00 = cosY * cosZ;
        res.m01 = -cosY * sinZ;
        res.m02 = sinY;
        res.m03 = 0;
        res.m10 = sinX * sinY * cosZ + cosX * sinZ;
        res.m11 = -sinX * sinY * sinZ + cosX * cosZ;
        res.m12 = -sinX * cosY;
        res.m13 = 0;
        res.m20 = -cosX * sinY * cosZ + sinX * sinZ;
        res.m21 = cosX * sinY * sinZ + sinX * cosZ;
        res.m22 = cosX * cosY;
        res.m23 = 0;
        res.m30 = 0;
        res.m31 = 0;
        res.m32 = 0;
        res.m33 = 1;

        return this->mulMatrix(res);
    }

    ZMat4 translate(float x, float y, float z)
    {
        ZMat4 res;

        res.m03 = x;
        res.m13 = y;
        res.m23 = z;

        return this->mulMatrix(res);
    }

    void toViewport(int x, int y, int w, int h, int depth)
    {
        m03 = x + w / 2.0f;
        m13 = y + h / 2.0f;
        m23 = depth / 2.0f;

        m00 = w / 2.0f;
        m11 = h / 2.0f;
        m22 = depth / 2.0f;
    }
};

#endif