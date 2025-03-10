#include <Arduino.h>

#include <fstream>
#include <strstream>

#include <zyngine.h>

#include "Audio.h"

#include <FS.h>
#include <SD.h>
#include <SPI.h>

// I2S Connections for Speaker
#define I2S_DOUT 40
#define I2S_BCLK 41
#define I2S_LRC 42

// SD CARD PINS
#define SD_SCK 36
#define SD_MISO 37
#define SD_MOSI 35
#define SD_CS 39

uint16_t rgb888_to_rgb565(uint8_t red8, uint8_t green8, uint8_t blue8, float dp = 1.0f)
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

class MyGame : public Zyngine
{
private:
    Mesh cubeMesh;
    Matrix4 projectionMatrix;

    Vector3 camera3D;

    float fTheta;
    char fps[7];

public:
    void onUserCreate() override
    {

        // cubeMesh.tris = {

        // // SOUTH
        // { 0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 0.0f },
        // { 0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f },

        // // EAST
        // { 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f },
        // { 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f },

        // // NORTH
        // { 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f },
        // { 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f },

        // // WEST
        // { 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f },
        // { 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f },

        // // TOP
        // { 0.0f, 1.0f, 0.0f,    0.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f },
        // { 0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f },

        // // BOTTOM
        // { 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f },
        // { 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f },

        // };

        // Load the cube mesh from an OBJ file
        if (!cubeMesh.loadFromObjectFile("/VideoShip.obj"))
        {
            Serial.println("Failed to load cube mesh");
            return;
        }

        float fNear = 0.1f;
        float fFar = 1000.0f;
        float fFov = 90.0f;
        float fAspectRatio = (float)screenHeight / (float)screenWidth;
        float fFovRad = 1.0f / tanf(fFov * 0.5f / 180.0f * PI);

        projectionMatrix.m[0][0] = fAspectRatio * fFovRad;
        projectionMatrix.m[1][1] = fFovRad;
        projectionMatrix.m[2][2] = fFar / (fFar - fNear);
        projectionMatrix.m[3][2] = (-fFar * fNear) / (fFar - fNear);
        projectionMatrix.m[2][3] = 1.0f;
        projectionMatrix.m[3][3] = 0.0f;
    }

    void onUserUpdate(float deltaTime) override
    {

        renderer->clear();
        sprintf(fps, "%.2f", (1.0f / deltaTime));
        renderer->printText(20, 20, fps, TFT_BLACK, TFT_WHITE);

        Matrix4 rotationMatrixZ, rotationMatrixX;
        // Set up rotation matrices
        fTheta += 0.5f * deltaTime;

        // Rotation Z
        rotationMatrixZ.m[0][0] = cosf(fTheta);
        rotationMatrixZ.m[0][1] = sinf(fTheta);
        rotationMatrixZ.m[1][0] = -sinf(fTheta);
        rotationMatrixZ.m[1][1] = cosf(fTheta);
        rotationMatrixZ.m[2][2] = 1;
        rotationMatrixZ.m[3][3] = 1;

        // Rotation X
        rotationMatrixX.m[0][0] = 1;
        rotationMatrixX.m[1][1] = cosf(fTheta * 0.5f);
        rotationMatrixX.m[1][2] = sinf(fTheta * 0.5f);
        rotationMatrixX.m[2][1] = -sinf(fTheta * 0.5f);
        rotationMatrixX.m[2][2] = cosf(fTheta * 0.5f);
        rotationMatrixX.m[3][3] = 1;

        std::vector<Triangle> trianglesToRender;

        for (auto tri : cubeMesh.tris)
        {
            Triangle projectedTriangle, translatedTriangle, rotatedTriangleZ, rotatedTriangleZX;

            multiplyMatrixVector(tri.vertices[0], rotatedTriangleZ.vertices[0], rotationMatrixZ);
            multiplyMatrixVector(tri.vertices[1], rotatedTriangleZ.vertices[1], rotationMatrixZ);
            multiplyMatrixVector(tri.vertices[2], rotatedTriangleZ.vertices[2], rotationMatrixZ);

            multiplyMatrixVector(rotatedTriangleZ.vertices[0], rotatedTriangleZX.vertices[0], rotationMatrixX);
            multiplyMatrixVector(rotatedTriangleZ.vertices[1], rotatedTriangleZX.vertices[1], rotationMatrixX);
            multiplyMatrixVector(rotatedTriangleZ.vertices[2], rotatedTriangleZX.vertices[2], rotationMatrixX);

            translatedTriangle = rotatedTriangleZX;
            translatedTriangle.vertices[0].z = tri.vertices[0].z + 10.0f;
            translatedTriangle.vertices[1].z = tri.vertices[1].z + 10.0f;
            translatedTriangle.vertices[2].z = tri.vertices[2].z + 10.0f;

            Vector3 normal, line1, line2;
            line1.x = translatedTriangle.vertices[1].x - translatedTriangle.vertices[0].x;
            line1.y = translatedTriangle.vertices[1].y - translatedTriangle.vertices[0].y;
            line1.z = translatedTriangle.vertices[1].z - translatedTriangle.vertices[0].z;

            line2.x = translatedTriangle.vertices[2].x - translatedTriangle.vertices[0].x;
            line2.y = translatedTriangle.vertices[2].y - translatedTriangle.vertices[0].y;
            line2.z = translatedTriangle.vertices[2].z - translatedTriangle.vertices[0].z;

            normal.x = line1.y * line2.z - line1.z * line2.y;
            normal.y = line1.z * line2.x - line1.x * line2.z;
            normal.z = line1.x * line2.y - line1.y * line2.x;

            // It's normally normal to normalise the normal
            float l = sqrtf(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
            normal.x /= l;
            normal.y /= l;
            normal.z /= l;

            if (
                normal.x * (translatedTriangle.vertices[0].x - camera3D.x) +
                    normal.y * (translatedTriangle.vertices[0].y - camera3D.y) +
                    normal.z * (translatedTriangle.vertices[0].z - camera3D.z) <
                0)
            {

                Vector3 lightDirection = {0.0f, 0.0f, -1.0f};

                float l = sqrtf(lightDirection.x * lightDirection.x + lightDirection.y * lightDirection.y + lightDirection.z * lightDirection.z);
                lightDirection.x /= l;
                lightDirection.y /= l;
                lightDirection.z /= l;

                // How similar is normal to light direction
                float dp = normal.x * lightDirection.x + normal.y * lightDirection.y + normal.z * lightDirection.z;

                uint16_t finalColor = rgb888_to_rgb565(255, 165, 0, dp);

                multiplyMatrixVector(translatedTriangle.vertices[0], projectedTriangle.vertices[0], projectionMatrix);
                multiplyMatrixVector(translatedTriangle.vertices[1], projectedTriangle.vertices[1], projectionMatrix);
                multiplyMatrixVector(translatedTriangle.vertices[2], projectedTriangle.vertices[2], projectionMatrix);
                // Scaling into view
                projectedTriangle.vertices[0].x += 1.0f;
                projectedTriangle.vertices[0].y += 1.0f;

                projectedTriangle.vertices[1].x += 1.0f;
                projectedTriangle.vertices[1].y += 1.0f;

                projectedTriangle.vertices[2].x += 1.0f;
                projectedTriangle.vertices[2].y += 1.0f;

                projectedTriangle.vertices[0].x *= 0.5f * (float)screenWidth;
                projectedTriangle.vertices[0].y *= 0.5f * (float)screenHeight;

                projectedTriangle.vertices[1].x *= 0.5f * (float)screenWidth;
                projectedTriangle.vertices[1].y *= 0.5f * (float)screenHeight;

                projectedTriangle.vertices[2].x *= 0.5f * (float)screenWidth;
                projectedTriangle.vertices[2].y *= 0.5f * (float)screenHeight;

                // renderer->fillTriangle(projectedTriangle.vertices[0].x, projectedTriangle.vertices[0].y, projectedTriangle.vertices[1].x, projectedTriangle.vertices[1].y, projectedTriangle.vertices[2].x, projectedTriangle.vertices[2].y, finalColor);
                // renderer->drawTriangle(projectedTriangle.vertices[0].x, projectedTriangle.vertices[0].y, projectedTriangle.vertices[1].x, projectedTriangle.vertices[1].y, projectedTriangle.vertices[2].x, projectedTriangle.vertices[2].y, TFT_BLACK);

                projectedTriangle.color = finalColor;
                trianglesToRender.push_back(projectedTriangle);
            }
        }

        // Sort triangles from back to front
        sort(trianglesToRender.begin(), trianglesToRender.end(), [](Triangle &t1, Triangle &t2)
             {
			float z1 = (t1.vertices[0].z + t1.vertices[1].z + t1.vertices[2].z) / 3.0f;
			float z2 = (t2.vertices[0].z + t2.vertices[1].z + t2.vertices[2].z) / 3.0f;
			return z1 > z2; });

        for (auto &projectedTriangle : trianglesToRender)
        {
            renderer->fillTriangle(projectedTriangle.vertices[0].x, projectedTriangle.vertices[0].y, projectedTriangle.vertices[1].x, projectedTriangle.vertices[1].y, projectedTriangle.vertices[2].x, projectedTriangle.vertices[2].y, projectedTriangle.color);
            // renderer->drawTriangle(projectedTriangle.vertices[0].x, projectedTriangle.vertices[0].y, projectedTriangle.vertices[1].x, projectedTriangle.vertices[1].y, projectedTriangle.vertices[2].x, projectedTriangle.vertices[2].y, TFT_BLACK);
        }
    }
};

MyGame game;
Audio audio;

void setup()
{
    Serial.begin(115200);
    SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
    if (!SD.begin(SD_CS))
    {
        Serial.println("Card Mount Failed");
    }

    // Setup I2S
    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);

    // Set Volume
    audio.setVolume(5);
    if (game.initialize(320, 480))
    {
        game.run();
    }
}

void loop()
{
}