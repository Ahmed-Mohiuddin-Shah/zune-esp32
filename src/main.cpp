#include <Arduino.h>

#include <zyngine.h>

#include "Audio.h"
// I2S Connections for Speaker
#define I2S_DOUT 40
#define I2S_BCLK 41
#define I2S_LRC 42

class MyGame : public Zyngine
{
private:
    Mesh cubeMesh;
    Triangle projectedTriangle;
    Matrix4 projectionMatrix, rotationMatrixZ, rotationMatrixX;
    float fTheta;
    char fps[7];

public:
    void onUserCreate() override
    {
        cubeMesh.tris = {
            // SOUTH
            {0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f},
            {0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f},

            // EAST
            {1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f},
            {1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f},

            // NORTH
            {1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f},
            {1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f},

            // WEST
            {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f},
            {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f},

            // TOP
            {0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f},
            {0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f},

            // BOTTOM
            {1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f},
            {1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f},
        };

        float fNear = 0.1f;
        float fFar = 1000.0f;
        float fFov = 75.0f;
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

        for (auto tri : cubeMesh.tris)
        {
            Triangle translatedTriangle, rotatedTriangleZ, rotatedTriangleZX;

            multiplyMatrixVector(tri.vertices[0], rotatedTriangleZ.vertices[0], rotationMatrixZ);
            multiplyMatrixVector(tri.vertices[1], rotatedTriangleZ.vertices[1], rotationMatrixZ);
            multiplyMatrixVector(tri.vertices[2], rotatedTriangleZ.vertices[2], rotationMatrixZ);

            multiplyMatrixVector(rotatedTriangleZ.vertices[0], rotatedTriangleZX.vertices[0], rotationMatrixX);
            multiplyMatrixVector(rotatedTriangleZ.vertices[1], rotatedTriangleZX.vertices[1], rotationMatrixX);
            multiplyMatrixVector(rotatedTriangleZ.vertices[2], rotatedTriangleZX.vertices[2], rotationMatrixX);

            translatedTriangle = rotatedTriangleZX;
            translatedTriangle.vertices[0].z = tri.vertices[0].z + 3.0f;
            translatedTriangle.vertices[1].z = tri.vertices[1].z + 3.0f;
            translatedTriangle.vertices[2].z = tri.vertices[2].z + 3.0f;

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

            renderer->drawTriangle(projectedTriangle.vertices[0].x, projectedTriangle.vertices[0].y, projectedTriangle.vertices[1].x, projectedTriangle.vertices[1].y, projectedTriangle.vertices[2].x, projectedTriangle.vertices[2].y, TFT_WHITE);
        
        }
    }
};

MyGame game;
Audio audio;

void setup()
{

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