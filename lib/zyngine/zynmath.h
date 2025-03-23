#ifndef ZYNMATH_H
#define ZYNMATH_H

#include <config_user.h>
#include <zyntexture.h>

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
#include <sstream>
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

struct Vector3
{
    float x = 0;
    float y = 0;
    float z = 1;
    // Constructor for easy initialization
    Vector2(float x = 0, float y = 0, float z = 1)
        : x(x), y(y), z(z) {}
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

struct Matrix4
{
    float m[4][4] = {0};
};

void multiplyMatrixVector(Vector4 &i, Vector4 &o, Matrix4 &m);
uint16_t RGB888ToRGB565(uint8_t red8, uint8_t green8, uint8_t blue8, float dp = 1.0f);
uint16_t RGBFloatToRGB565(float r, float g, float b);

// In this example:

// Ka represents ambient color.
// Kd represents diffuse color.
// Ks represents specular color.
// Ns represents shininess.
// d represents dissolve (transparency).
// map_Kd specifies the diffuse texture map.
struct ZynMaterial
{
    uint16_t Ka;
    uint16_t Kd;
    uint16_t Ks;
    uint16_t Ns;
    float d;
    ZynTexture *texture;
};
struct Triangle
{
    Vector4 vertices[3];
    Vector3 texture[3];
    int materialIndex;
    uint16_t color;


};

struct ZyngineMesh
{
    std::vector<Triangle> tris;
    ZynMaterial materials[10];
    ZynTexture textures[10];

    bool loadFromObjectFile(const char *fileName, bool hasTexture = false)
    {
        int materialIndex = -1;
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

        if (hasTexture)
        {
            std::string mtlFileName = std::string(fileName);
            size_t lastdot = mtlFileName.find_last_of(".");
            if (lastdot != std::string::npos)
            {
                mtlFileName = mtlFileName.substr(0, lastdot) + ".mtl";
            }

            FILE *file = fopen(mtlFileName.c_str(), "r");
            if (!file)
            {
                printf("Failed to open file for reading\n");
                return false;
            }

            while (!feof(file))
            {
                char line[128];

                // Clear the line buffer
                for (int i = 0; i < sizeof(line); i++)
                {
                    line[i] = '\0';
                }

                fgets(line, sizeof(line), file);

                if (line[0] == 'n')
                {
                    materialIndex++;
                    ZynMaterial mat;
                    materials[materialIndex] = mat;
                }
                else if (line[0] == 'K')
                {
                    if (line[1] == 'a')
                    {
                        float r, g, b;
                        sscanf(line, "Ka %f %f %f", &r, &g, &b);
                        materials[materialIndex].Ka = RGBFloatToRGB565(r, g, b);
                    }
                    if (line[1] == 'd')
                    {
                        float r, g, b;
                        sscanf(line, "Kd %f %f %f", &r, &g, &b);
                        materials[materialIndex].Kd = RGBFloatToRGB565(r, g, b);
                    }
                    if (line[1] == 's')
                    {
                        float r, g, b;
                        sscanf(line, "Kd %d", &r, &g, &b);
                        materials[materialIndex].Ks = RGBFloatToRGB565(r, g, b);
                    }
                }
                else if (line[0] == 'N')
                {
                    sscanf(line, "Ns %d", &materials[materialIndex].Ns);
                }
                else if (line[0] == 'd')
                {
                    sscanf(line, "d %f", &materials[materialIndex].d);
                }
                else if (line[0] == 'm')
                {
                    char name[30];
                    sscanf(line, "map_Kd %s", &name);
                    std::string imageFileName = std::string(name);
                    size_t lastdot = imageFileName.find_last_of(".");
                    if (lastdot != std::string::npos)
                    {
                        imageFileName = "./resources/optimized_assets/3d_models/" + imageFileName.substr(0, lastdot) + ".zyntex";
                    }
                    textures[materialIndex] = ZynTexture(name, imageFileName.c_str());
                }
            }
            fclose(file);
        }

        // for (const auto &material : materials)
        // {
        //     printf("Material:\n");
        //     printf("  Ka: %u\n", material.Ka);
        //     printf("  Kd: %u\n", material.Kd);
        //     printf("  Ks: %u\n", material.Ks);
        //     printf("  Ns: %u\n", material.Ns);
        //     printf("  d: %f\n", material.d);
        //     if (material.texture)
        //     {
        //         printf("  Texture: %s\n", material.texture->name.c_str());
        //     }
        // }

        FILE *file = fopen(fileName, "r");
        if (!file)
        {
            printf("Failed to open file for reading\n");
            return false;
        }
        std::vector<Vector4> vertices;
        std::vector<Vector3> textures;
        int currentTexture = -1;

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
                if (line[1] == 't')
                {
                    Vector3 texture;
                    sscanf(line, "vt %f %f", &texture.x, &texture.y);
                    texture.z = 1.0f;
                    textures.push_back(texture);
                }
                else
                {
                    Vector4 vertex;
                    sscanf(line, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
                    vertex.w = 1.0f;
                    vertices.push_back(vertex);
                }
            }
            else if (line[0] == 'u')
            {
                currentTexture++;
            }
            else if (line[0] == 'f')
            {
                if (!hasTexture)
                {
                    Triangle triangle;
                    int v1, v2, v3;
                    sscanf(line, "f %d %d %d", &v1, &v2, &v3);

                    triangle.vertices[0] = vertices[v1 - 1];
                    triangle.vertices[1] = vertices[v2 - 1];
                    triangle.vertices[2] = vertices[v3 - 1];
                    tris.push_back(triangle);
                }
                else
                {
                    Triangle triangle;
                    int v1, vt1, v2, vt2, v3, vt3;
                    sscanf(line, "f %d/%d %d/%d %d/%d", &v1, &vt1, &v2, &vt2, &v3, &vt3);

                    triangle.vertices[0] = vertices[v1 - 1];
                    triangle.vertices[1] = vertices[v2 - 1];
                    triangle.vertices[2] = vertices[v3 - 1];
                    triangle.texture[0] = textures[vt1 - 1];
                    triangle.texture[1] = textures[vt2 - 1];
                    triangle.texture[2] = textures[vt3 - 1];
                    triangle.materialIndex = currentTexture;

                    tris.push_back(triangle);
                }
            }
        }
        fclose(file);
        return true;
#endif
    }
};

#endif