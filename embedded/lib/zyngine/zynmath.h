#ifndef ZYNMATH_H
#define ZYNMATH_H

#include <vector>

#ifndef PI
#define PI 3.14159265358979323846f
#endif
#ifndef DEG2RAD
#define DEG2RAD (PI / 180.0f)
#endif
#ifndef RAD2DEG
#define RAD2DEG (180.0f / PI)
#endif

// Math.h
struct Vector2
{
    float x, y;
};

struct Vector3
{
    float x, y, z;
};

struct Triangle
{
    Vector3 vertices[3];
};

struct Mesh
{
    std::vector<Triangle> tris;
};

struct Matrix4
{
    float m[4][4];
};

void multiplyMatrixVector(Vector3 &i, Vector3 &o, Matrix4 &m);

#endif