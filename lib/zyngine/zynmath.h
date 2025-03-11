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

#include <FS.h>
#include <SD.h>
#include <SPI.h>

// Math.h
struct Vector2
{
    float x;
    float y;
    // Constructor for easy initialization
    Vector2(float x = 0, float y = 0)
        : x(x), y(y) {}
};

struct Vector3
{
    float x = 0;
    float y = 0;
    float z = 0;
    float w = 1;

    // Constructor for easy initialization
    Vector3(float x = 0, float y = 0, float z = 0, float w = 1)
        : x(x), y(y), z(z), w(w) {}
};

struct Triangle
{
    Vector3 vertices[3];
    uint16_t color;
};

struct Mesh
{
    std::vector<Triangle> tris;

    bool loadFromObjectFile(const char *fileName)
    {
        File file = SD.open(fileName);
        if (!file)
        {
            Serial.println("Failed to open file for reading");
            return false;
        }

        std::vector<Vector3> vertices;

        while (file.available())
        {
            char line[128];

            // Clear the line buffer
            for (int i = 0; i < sizeof(line); i++)
            {
                line[i] = '\0';
            }

            file.readBytesUntil('\n', line, sizeof(line) - 1);
            if (line[0] == 'v')
            {
                Vector3 vertex;
                sscanf(line, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
                // Serial.println(line);
                vertices.push_back(vertex);
            }
            else if (line[0] == 'f')
            {
                Triangle triangle;
                int v1, v2, v3;
                sscanf(line, "f %d %d %d", &v1, &v2, &v3);
                // Serial.println(line);

                triangle.vertices[0] = vertices[v1 - 1];
                triangle.vertices[1] = vertices[v2 - 1];
                triangle.vertices[2] = vertices[v3 - 1];
                tris.push_back(triangle);
            }
        }
        file.close();
        return true;
    }
};

struct Matrix4
{
    float m[4][4] = {0};
};

void multiplyMatrixVector(Vector3 &i, Vector3 &o, Matrix4 &m);
uint16_t RGB888ToRGB565(uint8_t red8, uint8_t green8, uint8_t blue8, float dp = 1.0f);

#endif