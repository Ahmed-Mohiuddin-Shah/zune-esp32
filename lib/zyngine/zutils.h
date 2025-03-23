#ifndef ZUTILS_H
#define ZUTILS_H

#include <config_user.h>

#ifdef ZYNGINE_ESP32S3
#include <Arduino.h>
#endif

#include <string>
#include <fstream>
#include <strstream>

#include <zynmath.h>


uint16_t rgb888_to_rgb565(uint8_t red8, uint8_t green8, uint8_t blue8, float dp = 1.0f);
Matrix4 createProjectionMatrix(int screenWidth, int screenHeight, float fov = 90.0f, float nearPlane = 0.1f, float farPlane = 100.0f);
Vector4 matrixMultiplyVector(Matrix4 &m, Vector4 &i);
Matrix4 matrixMakeIdentity();
Matrix4 matrixMakeRotationX(float fAngleRad);
Matrix4 matrixMakeRotationY(float fAngleRad);
Matrix4 matrixMakeRotationZ(float fAngleRad);
Matrix4 matrixMakeTranslation(float x, float y, float z);
Matrix4 matrixMakeProjection(float fFovDegrees, float fAspectRatio, float fNear, float fFar);
Matrix4 matrixMultiplyMatrix(Matrix4 &m1, Matrix4 &m2);
Matrix4 matrixPointAt(Vector4 &pos, Vector4 &target, Vector4 &up);
Matrix4 matrixQuickInverse(Matrix4 &m);
Vector4 Vector_Add(Vector4 &v1, Vector4 &v2);
Vector4 Vector_Sub(Vector4 &v1, Vector4 &v2);
Vector4 Vector_Mul(Vector4 &v1, float k);
Vector4 Vector_Div(Vector4 &v1, float k);
float Vector_DotProduct(Vector4 &v1, Vector4 &v2);
float Vector_Length(Vector4 &v);
Vector4 Vector_Normalise(Vector4 &v);
Vector4 Vector_CrossProduct(Vector4 &v1, Vector4 &v2);
Vector4 Vector_IntersectPlane(Vector4 &plane_p, Vector4 &plane_n, Vector4 &lineStart, Vector4 &lineEnd);
int triangleClipAgainstPlane(Vector4 plane_p, Vector4 plane_n, Triangle &in_tri, Triangle &out_tri1, Triangle &out_tri2);

// Utils.h
class Zutils
{
public:
    static void Log(const std::string &message);
    static float GetDeltaTime();
};

uint16_t rgb888_to_rgb565(uint8_t red8, uint8_t green8, uint8_t blue8, float dp)
{
    red8 = floor(red8 * dp);
    green8 = floor(green8 * dp);
    blue8 = floor(blue8 * dp);

    // Convert 8-bit red to 5-bit red.
    uint16_t red5 = (red8 * 31) / 255;
    // Convert 8-bit green to 6-bit green.
    uint16_t green6 = (green8 * 63) / 255;
    // Convert 8-bit blue to 5-bit blue.
    uint16_t blue5 = (blue8 * 31) / 255;

    // Shift the red value to the left by 11 bits.
    uint16_t red5_shifted = red5 << 11;
    // Shift the green value to the left by 5 bits.
    uint16_t green6_shifted = green6 << 5;

    // Combine the red, green, and blue values.
    uint16_t rgb565 = red5_shifted | green6_shifted | blue5;

    return rgb565;

}

Matrix4 createProjectionMatrix(int screenWidth, int screenHeight, float fov, float nearPlane, float farPlane)
{
    Matrix4 projectionMatrix;
    float fAspectRatio = (float)screenHeight / (float)screenWidth;
    float fovRad = 1.0f / tanf(fov * 0.5f / 180.0f * PI);

    projectionMatrix.m[0][0] = fAspectRatio * fovRad;
    projectionMatrix.m[1][1] = fovRad;
    projectionMatrix.m[2][2] = farPlane / (farPlane - nearPlane);
    projectionMatrix.m[3][2] = (-farPlane * nearPlane) / (farPlane - nearPlane);
    projectionMatrix.m[2][3] = 1.0f;
    projectionMatrix.m[3][3] = 0.0f;

    return projectionMatrix;
}

Vector4 matrixMultiplyVector(Matrix4 &m, Vector4 &i)
{
    Vector4 v;
    v.w = 1.0f;
    i.w = 1.0f;
    v.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + i.w * m.m[3][0];
    v.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + i.w * m.m[3][1];
    v.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + i.w * m.m[3][2];
    v.w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + i.w * m.m[3][3];
    return v;
}

Matrix4 matrixMakeIdentity()
{
    Matrix4 matrix;
    matrix.m[0][0] = 1.0f;
    matrix.m[1][1] = 1.0f;
    matrix.m[2][2] = 1.0f;
    matrix.m[3][3] = 1.0f;
    return matrix;
}

Matrix4 matrixMakeRotationX(float fAngleRad)
{
    Matrix4 matrix;
    matrix.m[0][0] = 1.0f;
    matrix.m[1][1] = cosf(fAngleRad);
    matrix.m[1][2] = sinf(fAngleRad);
    matrix.m[2][1] = -sinf(fAngleRad);
    matrix.m[2][2] = cosf(fAngleRad);
    matrix.m[3][3] = 1.0f;
    return matrix;
}

Matrix4 matrixMakeRotationY(float fAngleRad)
{
    Matrix4 matrix;
    matrix.m[0][0] = cosf(fAngleRad);
    matrix.m[0][2] = sinf(fAngleRad);
    matrix.m[2][0] = -sinf(fAngleRad);
    matrix.m[1][1] = 1.0f;
    matrix.m[2][2] = cosf(fAngleRad);
    matrix.m[3][3] = 1.0f;
    return matrix;
}

Matrix4 matrixMakeRotationZ(float fAngleRad)
{
    Matrix4 matrix;
    matrix.m[0][0] = cosf(fAngleRad);
    matrix.m[0][1] = sinf(fAngleRad);
    matrix.m[1][0] = -sinf(fAngleRad);
    matrix.m[1][1] = cosf(fAngleRad);
    matrix.m[2][2] = 1.0f;
    matrix.m[3][3] = 1.0f;
    return matrix;
}

Matrix4 matrixMakeTranslation(float x, float y, float z)
{
    Matrix4 matrix;
    matrix.m[0][0] = 1.0f;
    matrix.m[1][1] = 1.0f;
    matrix.m[2][2] = 1.0f;
    matrix.m[3][3] = 1.0f;
    matrix.m[3][0] = x;
    matrix.m[3][1] = y;
    matrix.m[3][2] = z;
    return matrix;
}

Matrix4 matrixMakeProjection(float fFovDegrees, float fAspectRatio, float fNear, float fFar)
{
    float fFovRad = 1.0f / tanf(fFovDegrees * 0.5f / 180.0f * 3.14159f);
    Matrix4 matrix;
    matrix.m[0][0] = fAspectRatio * fFovRad;
    matrix.m[1][1] = fFovRad;
    matrix.m[2][2] = fFar / (fFar - fNear);
    matrix.m[3][2] = (-fFar * fNear) / (fFar - fNear);
    matrix.m[2][3] = 1.0f;
    matrix.m[3][3] = 0.0f;
    return matrix;
}

Matrix4 matrixMultiplyMatrix(Matrix4 &m1, Matrix4 &m2)
{
    Matrix4 matrix;
    for (int c = 0; c < 4; c++)
        for (int r = 0; r < 4; r++)
            matrix.m[r][c] = m1.m[r][0] * m2.m[0][c] + m1.m[r][1] * m2.m[1][c] + m1.m[r][2] * m2.m[2][c] + m1.m[r][3] * m2.m[3][c];
    return matrix;
}

Matrix4 matrixPointAt(Vector4 &pos, Vector4 &target, Vector4 &up)
{
    // Calculate new forward direction
    Vector4 newForward = Vector_Sub(target, pos);
    newForward = Vector_Normalise(newForward);

    // Calculate new Up direction
    Vector4 a = Vector_Mul(newForward, Vector_DotProduct(up, newForward));
    Vector4 newUp = Vector_Sub(up, a);
    newUp = Vector_Normalise(newUp);

    // New Right direction is easy, its just cross product
    Vector4 newRight = Vector_CrossProduct(newUp, newForward);

    // Construct Dimensioning and Translation Matrix
    Matrix4 matrix;
    matrix.m[0][0] = newRight.x;
    matrix.m[0][1] = newRight.y;
    matrix.m[0][2] = newRight.z;
    matrix.m[0][3] = 0.0f;
    matrix.m[1][0] = newUp.x;
    matrix.m[1][1] = newUp.y;
    matrix.m[1][2] = newUp.z;
    matrix.m[1][3] = 0.0f;
    matrix.m[2][0] = newForward.x;
    matrix.m[2][1] = newForward.y;
    matrix.m[2][2] = newForward.z;
    matrix.m[2][3] = 0.0f;
    matrix.m[3][0] = pos.x;
    matrix.m[3][1] = pos.y;
    matrix.m[3][2] = pos.z;
    matrix.m[3][3] = 1.0f;
    return matrix;
}

Matrix4 matrixQuickInverse(Matrix4 &m) // Only for Rotation/Translation Matrices
{
    Matrix4 matrix;
    matrix.m[0][0] = m.m[0][0];
    matrix.m[0][1] = m.m[1][0];
    matrix.m[0][2] = m.m[2][0];
    matrix.m[0][3] = 0.0f;
    matrix.m[1][0] = m.m[0][1];
    matrix.m[1][1] = m.m[1][1];
    matrix.m[1][2] = m.m[2][1];
    matrix.m[1][3] = 0.0f;
    matrix.m[2][0] = m.m[0][2];
    matrix.m[2][1] = m.m[1][2];
    matrix.m[2][2] = m.m[2][2];
    matrix.m[2][3] = 0.0f;
    matrix.m[3][0] = -(m.m[3][0] * matrix.m[0][0] + m.m[3][1] * matrix.m[1][0] + m.m[3][2] * matrix.m[2][0]);
    matrix.m[3][1] = -(m.m[3][0] * matrix.m[0][1] + m.m[3][1] * matrix.m[1][1] + m.m[3][2] * matrix.m[2][1]);
    matrix.m[3][2] = -(m.m[3][0] * matrix.m[0][2] + m.m[3][1] * matrix.m[1][2] + m.m[3][2] * matrix.m[2][2]);
    matrix.m[3][3] = 1.0f;
    return matrix;
}

Vector4 Vector_Add(Vector4 &v1, Vector4 &v2)
{
    return {v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, 1.0f};
}

Vector4 Vector_Sub(Vector4 &v1, Vector4 &v2)
{
    return {v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, 1.0f};
}

Vector4 Vector_Mul(Vector4 &v1, float k)
{
    return {v1.x * k, v1.y * k, v1.z * k, 1.0f};
}

Vector4 Vector_Div(Vector4 &v1, float k)
{
    return {v1.x / k, v1.y / k, v1.z / k, 1.0f};
}

float Vector_DotProduct(Vector4 &v1, Vector4 &v2)
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

float Vector_Length(Vector4 &v)
{
    return sqrtf(Vector_DotProduct(v, v));
}

Vector4 Vector_Normalise(Vector4 &v)
{
    float l = Vector_Length(v);
    return {v.x / l, v.y / l, v.z / l, 1.0f};
}

Vector4 Vector_CrossProduct(Vector4 &v1, Vector4 &v2)
{
    Vector4 v;
    v.x = v1.y * v2.z - v1.z * v2.y;
    v.y = v1.z * v2.x - v1.x * v2.z;
    v.z = v1.x * v2.y - v1.y * v2.x;
    return v;
}

Vector4 Vector_IntersectPlane(Vector4 &plane_p, Vector4 &plane_n, Vector4 &lineStart, Vector4 &lineEnd)
{
    plane_n = Vector_Normalise(plane_n);
    float plane_d = -Vector_DotProduct(plane_n, plane_p);
    float ad = Vector_DotProduct(lineStart, plane_n);
    float bd = Vector_DotProduct(lineEnd, plane_n);
    float t = (-plane_d - ad) / (bd - ad);
    Vector4 lineStartToEnd = Vector_Sub(lineEnd, lineStart);
    Vector4 lineToIntersect = Vector_Mul(lineStartToEnd, t);
    return Vector_Add(lineStart, lineToIntersect);
}

int triangleClipAgainstPlane(Vector4 plane_p, Vector4 plane_n, Triangle &in_tri, Triangle &out_tri1, Triangle &out_tri2)
{
    // Make sure plane normal is indeed normal
    plane_n = Vector_Normalise(plane_n);

    // Return signed shortest distance from point to plane, plane normal must be normalised
    auto dist = [&](Vector4 &p)
    {
        Vector4 n = Vector_Normalise(p);
        return (plane_n.x * p.x + plane_n.y * p.y + plane_n.z * p.z - Vector_DotProduct(plane_n, plane_p));
    };

    // Create two temporary storage arrays to classify points either side of plane
    // If distance sign is positive, point lies on "inside" of plane
    Vector4 *inside_points[3];
    int nInsidePointCount = 0;
    Vector4 *outside_points[3];
    int nOutsidePointCount = 0;

    // Get signed distance of each point in triangle to plane
    float d0 = dist(in_tri.vertices[0]);
    float d1 = dist(in_tri.vertices[1]);
    float d2 = dist(in_tri.vertices[2]);

    if (d0 >= 0)
    {
        inside_points[nInsidePointCount++] = &in_tri.vertices[0];
    }
    else
    {
        outside_points[nOutsidePointCount++] = &in_tri.vertices[0];
    }
    if (d1 >= 0)
    {
        inside_points[nInsidePointCount++] = &in_tri.vertices[1];
    }
    else
    {
        outside_points[nOutsidePointCount++] = &in_tri.vertices[1];
    }
    if (d2 >= 0)
    {
        inside_points[nInsidePointCount++] = &in_tri.vertices[2];
    }
    else
    {
        outside_points[nOutsidePointCount++] = &in_tri.vertices[2];
    }

    // Now classify triangle points, and break the input triangle into
    // smaller output triangles if required. There are four possible
    // outcomes...

    if (nInsidePointCount == 0)
    {
        // All points lie on the outside of plane, so clip whole triangle
        // It ceases to exist

        return 0; // No returned triangles are valid
    }

    if (nInsidePointCount == 3)
    {
        // All points lie on the inside of plane, so do nothing
        // and allow the triangle to simply pass through
        out_tri1 = in_tri;

        return 1; // Just the one returned original triangle is valid
    }

    if (nInsidePointCount == 1 && nOutsidePointCount == 2)
    {
        // Triangle should be clipped. As two points lie outside
        // the plane, the triangle simply becomes a smaller triangle

        // Copy appearance info to new triangle
        out_tri1.color = in_tri.color;

        // The inside point is valid, so keep that...
        out_tri1.vertices[0] = *inside_points[0];

        // but the two new points are at the locations where the
        // original sides of the triangle (lines) intersect with the plane
        out_tri1.vertices[1] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0]);
        out_tri1.vertices[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[1]);

        return 1; // Return the newly formed single triangle
    }

    if (nInsidePointCount == 2 && nOutsidePointCount == 1)
    {
        // Triangle should be clipped. As two points lie inside the plane,
        // the clipped triangle becomes a "quad". Fortunately, we can
        // represent a quad with two new triangles

        // Copy appearance info to new triangles
        out_tri1.color = in_tri.color;

        out_tri2.color = in_tri.color;

        // The first triangle consists of the two inside points and a new
        // point determined by the location where one side of the triangle
        // intersects with the plane
        out_tri1.vertices[0] = *inside_points[0];
        out_tri1.vertices[1] = *inside_points[1];
        out_tri1.vertices[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0]);

        // The second triangle is composed of one of he inside points, a
        // new point determined by the intersection of the other side of the
        // triangle and the plane, and the newly created point above
        out_tri2.vertices[0] = *inside_points[1];
        out_tri2.vertices[1] = out_tri1.vertices[2];
        out_tri2.vertices[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[1], *outside_points[0]);

        return 2; // Return two newly formed triangles which form a quad
    }
    // If the function reaches here, it means no valid triangles were formed
    return 0;
}

#endif