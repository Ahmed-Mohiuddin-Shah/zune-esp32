#ifndef ZYNMATH_H
#define ZYNMATH_H

#include <config_user.h>
#include <zyntexture.h>


// Check cross compatibility later
#include <cmath>
#include <iostream>

#ifndef PI
#define PI 3.14159265358979323846f
#endif
#ifndef DEG2RAD
#define DEG2RAD (PI / 180.0f)
#endif
#ifndef RAD2DEG
#define RAD2DEG (180.0f / PI)
#endif

#ifdef ZYNGINE_ESP32S3
#include <Arduino.h>
#include <FS.h>
#include <SD.h>
#include <SPI.h>
#endif

#ifdef ZYNGINE_WINDOWS_NATIVE_RAYLIB_CUSTOM_SOFTWARE_RENDERER

#endif


struct Vec2
{
    float x;
    float y;

    Vec2(float x = 0.0f, float y = 0.0f) : x(x), y(y) {}

    float normalize()
    {
        float length = getLength();
        if (length != 0.0f)
        {
            float invLength = 1.0f / length;
            x *= invLength;
            y *= invLength;
        }
        return length;
    }

    Vec2 normalized() const
    {
        float invLength = 1.0f / getLength();
        return Vec2(x * invLength, y * invLength);
    }

    float getLength() const
    {
        return std::sqrt(x * x + y * y);
    }

    float dot(const Vec2 &v) const
    {
        return x * v.x + y * v.y;
    }

    float cross(const Vec2 &v) const
    {
        return y * v.x - x * v.y;
    }

    Vec2 add(const Vec2 &v) const
    {
        return Vec2(x + v.x, y + v.y);
    }

    Vec2 sub(const Vec2 &v) const
    {
        return Vec2(x - v.x, y - v.y);
    }

    Vec2 div(float v) const
    {
        return Vec2(x / v, y / v);
    }

    Vec2 mul(float v) const
    {
        return Vec2(x * v, y * v);
    }

    bool equals(const Vec2 &v) const
    {
        return x == v.x && y == v.y;
    }

    // Operator overloads for more natural syntax
    Vec2 operator+(const Vec2 &v) const { return add(v); }
    Vec2 operator-(const Vec2 &v) const { return sub(v); }
    Vec2 operator/(float v) const { return div(v); }
    Vec2 operator*(float v) const { return mul(v); }
    bool operator==(const Vec2 &v) const { return equals(v); }
    bool operator!=(const Vec2 &v) const { return !equals(v); }
};

struct Vec3
{
    float x;
    float y;
    float z;

    Vec3(float x = 0.0f, float y = 0.0f, float z = 0.0f) : x(x), y(y), z(z) {}

    float normalize()
    {
        float length = getLength();
        if (length != 0.0f)
        {
            float invLength = 1.0f / length;
            x *= invLength;
            y *= invLength;
            z *= invLength;
        }
        return length;
    }

    Vec3 normalized() const
    {
        float invLength = 1.0f / getLength();
        return Vec3(x * invLength, y * invLength, z * invLength);
    }

    float getLength() const
    {
        return std::sqrt(x * x + y * y + z * z);
    }

    float dot(const Vec3 &v) const
    {
        return x * v.x + y * v.y + z * v.z;
    }

    Vec3 cross(const Vec3 &v) const
    {
        return Vec3(
            y * v.z - z * v.y,
            z * v.x - x * v.z,
            x * v.y - y * v.x);
    }

    Vec3 add(const Vec3 &v) const
    {
        return Vec3(x + v.x, y + v.y, z + v.z);
    }

    Vec3 sub(const Vec3 &v) const
    {
        return Vec3(x - v.x, y - v.y, z - v.z);
    }

    Vec3 div(float v) const
    {
        return Vec3(x / v, y / v, z / v);
    }

    Vec3 divXYZ(float x, float y, float z) const
    {
        return Vec3(this->x / x, this->y / y, this->z / z);
    }

    Vec3 mul(float v) const
    {
        return Vec3(x * v, y * v, z * v);
    }

    Vec3 mulXYZ(float x, float y, float z) const
    {
        return Vec3(this->x * x, this->y * y, this->z * z);
    }

    bool equals(const Vec3 &v) const
    {
        return x == v.x && y == v.y && z == v.z;
    }

    // Operator overloads for more natural syntax
    Vec3 operator+(const Vec3 &v) const { return add(v); }
    Vec3 operator-(const Vec3 &v) const { return sub(v); }
    Vec3 operator/(float v) const { return div(v); }
    Vec3 operator*(float v) const { return mul(v); }
    bool operator==(const Vec3 &v) const { return equals(v); }
    bool operator!=(const Vec3 &v) const { return !equals(v); }
};

struct Mat4
{
    float m00, m01, m02, m03;
    float m10, m11, m12, m13;
    float m20, m21, m22, m23;
    float m30, m31, m32, m33;

    Mat4()
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

    static Mat4 fromAxis(const Vec3 &vx, const Vec3 &vy, const Vec3 &vz)
    {
        Mat4 res;

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

    Mat4 mulMatrix(const Mat4 &right) const
    {
        Mat4 res;

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

    Vec3 mulVector(const Vec3 &right, float w = 1.0f) const
    {
        Vec3 res;

        res.x = m00 * right.x + m01 * right.y + m02 * right.z + m03 * w;
        res.y = m10 * right.x + m11 * right.y + m12 * right.z + m13 * w;
        res.z = m20 * right.x + m21 * right.y + m22 * right.z + m23 * w;

        return res;
    }

    Mat4 scale(float x, float y, float z) const
    {
        Mat4 scaleMat;
        scaleMat.m00 = x;
        scaleMat.m11 = y;
        scaleMat.m22 = z;

        return this->mulMatrix(scaleMat);
    }

    Mat4 scale(float uniformScale) const
    {
        return this->scale(uniformScale, uniformScale, uniformScale);
    }

    Mat4 rotate(float x, float y, float z) const
    {
        const float sinX = std::sin(x);
        const float cosX = std::cos(x);
        const float sinY = std::sin(y);
        const float cosY = std::cos(y);
        const float sinZ = std::sin(z);
        const float cosZ = std::cos(z);

        Mat4 rotation;

        rotation.m00 = cosY * cosZ;
        rotation.m01 = -cosY * sinZ;
        rotation.m02 = sinY;
        rotation.m03 = 0;

        rotation.m10 = sinX * sinY * cosZ + cosX * sinZ;
        rotation.m11 = -sinX * sinY * sinZ + cosX * cosZ;
        rotation.m12 = -sinX * cosY;
        rotation.m13 = 0;

        rotation.m20 = -cosX * sinY * cosZ + sinX * sinZ;
        rotation.m21 = cosX * sinY * sinZ + sinX * cosZ;
        rotation.m22 = cosX * cosY;
        rotation.m23 = 0;

        rotation.m30 = 0;
        rotation.m31 = 0;
        rotation.m32 = 0;
        rotation.m33 = 1;

        return this->mulMatrix(rotation);
    }

    Mat4 translate(float x, float y, float z) const
    {
        Mat4 translation;

        translation.m03 = x;
        translation.m13 = y;
        translation.m23 = z;

        return this->mulMatrix(translation);
    }

    // Operator overloads
    Mat4 operator*(const Mat4 &right) const { return mulMatrix(right); }
    Vec3 operator*(const Vec3 &right) const { return mulVector(right); }
};

#endif