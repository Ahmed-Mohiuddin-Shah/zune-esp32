#ifndef ZYNMATH_H
#define ZYNMATH_H

#include <config_user.h>

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

#ifdef ZYNGINE_ESP32S3
#include <Arduino.h>
#include <FS.h>
#include <SD.h>
#include <SPI.h>
#endif

#ifdef ZYNGINE_WINDOWS_NATIVE_RAYLIB_CUSTOM_SOFTWARE_RENDERER
#include <cstdint>
#include <cmath>

#include <stdio.h>
#include <dirent.h>
#include <cstdio>
#include <raylib/raymath.h>
#endif

#ifdef ZYNGINE_ESP32S3
struct Vector2
{
    float x;
    float y;
    // Constructor for easy initialization
    Vector2(float x = 0, float y = 0)
        : x(x), y(y) {}
};

struct Vector4
{
    float x = 0;
    float y = 0;
    float z = 0;
    float w = 1;

    // Constructor for easy initialization
    Vector4(float x = 0, float y = 0, float z = 0, float w = 1)
        : x(x), y(y), z(z), w(w) {}
};
#endif

struct Triangle
{
    Vector4 vertices[3];
    uint16_t color;
};

struct ZyngineMesh
{
    std::vector<Triangle> tris;

    bool loadFromObjectFile(const char *fileName)
    {
#ifdef ZYNGINE_ESP32S3
        File file = SD.open(fileName);
        if (!file)
        {
            Serial.println("Failed to open file for reading");
            return false;
        }

        std::vector<Vector4> vertices;

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
                Vector4 vertex;
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
#endif
#ifdef ZYNGINE_WINDOWS_NATIVE_RAYLIB_CUSTOM_SOFTWARE_RENDERER

        FILE *file = fopen(fileName, "r");
        if (!file)
        {
            printf("Failed to open file for reading\n");
            return false;
        }
        std::vector<Vector4> vertices;

        while (!feof(file))
        {
            char line[128];

            // Clear the line buffer
            for (int i = 0; i < sizeof(line); i++)
            {
                line[i] = '\0';
            }

            fgets(line, sizeof(line), file);
            if (line[0] == 'v')
            {
                Vector4 vertex;
                sscanf(line, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
                vertices.push_back(vertex);
            }
            else if (line[0] == 'f')
            {
                Triangle triangle;
                int v1, v2, v3;
                sscanf(line, "f %d %d %d", &v1, &v2, &v3);

                triangle.vertices[0] = vertices[v1 - 1];
                triangle.vertices[1] = vertices[v2 - 1];
                triangle.vertices[2] = vertices[v3 - 1];
                tris.push_back(triangle);
            }
        }
        fclose(file);
        return true;
#endif
    }
};

struct Matrix4
{
    float m[4][4] = {0};
};

void multiplyMatrixVector(Vector4 &i, Vector4 &o, Matrix4 &m);
uint16_t RGB888ToRGB565(uint8_t red8, uint8_t green8, uint8_t blue8, float dp = 1.0f);

#endif