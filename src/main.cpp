#include <zyngine.h>
#include <zutils.h>

#ifdef ZYNGINE_ESP32S3
#include <Arduino.h>
#include "Audio.h"
#include "ESP32Encoder.h"

#include <FS.h>
#include <SD.h>
#include <SPI.h>
ESP32Encoder encoder;
Audio audio;
// I2S Connections for Speaker
#define I2S_DOUT 40
#define I2S_BCLK 41
#define I2S_LRC 42

// SD CARD PINS
#define SD_SCK 36
#define SD_MISO 37
#define SD_MOSI 35
#define SD_CS 39

#define CLK 18 // CLK ENCODER
#define DT 17  // DT ENCODER
#define ENCODER_BUTTON 43
#endif
#ifndef ZYNGINE_ESP32S3
#include <math.h>
#include <stdio.h>
#endif

#include <fstream>
#include <strstream>

#include <list>
#include <algorithm>

class MyGame : public Zyngine
{
private:
    ZyngineMesh cubeMesh;
    Matrix4 projectionMatrix;

    Vector4 cameraPosition;
    Vector4 cameraLookDirection;

    float yaw;
    int toggle = 0;

    float theta;
    char fps[50];
    int32_t count;
    bool skipFrame = true;

public:
    void
    onUserCreate() override
    {

        // Load the cube mesh from an OBJ file
        if (!cubeMesh.loadFromObjectFile("./resources/3d-models/mountains.obj"))
        {
#ifdef ZYNGINE_ESP32S3
            Serial.println("Failed to load cube mesh");
            return;
#endif
        }

        projectionMatrix = createProjectionMatrix(screenWidth, screenHeight, 90.0f, 0.1f, 100.0f);

        cameraPosition = {0.0f, 0.0f, 5.0f};

#ifdef ZYNGINE_ESP32S3
        encoder.attachFullQuad(CLK, DT);
        encoder.setCount(0);
        pinMode(ENCODER_BUTTON, INPUT_PULLUP);
#endif
    }

    void onUserUpdate(float deltaTime) override
    {

#ifdef ZYNGINE_ESP32S3
        if (digitalRead(ENCODER_BUTTON) == LOW)
        {
            toggle = (toggle + 1) % 3;
        }
        count = encoder.getCount();

        if (count != 0)
        {
            if (toggle == 0)
            {
                cameraPosition.x += count * deltaTime;
            }
            else if (toggle == 1)
            {
                cameraPosition.y += count * deltaTime;
            }
            else
            {
                cameraPosition.z += count * deltaTime;
            }
        }
        encoder.setCount(0);
#endif

#ifdef ZYNGINE_WINDOWS_NATIVE_RAYLIB_CUSTOM_SOFTWARE_RENDERER

        if (!skipFrame)
        {
            if (IsKeyDown(KEY_A))
                cameraPosition.x -= 30 * deltaTime;
            if (IsKeyDown(KEY_D))
                cameraPosition.x += 30 * deltaTime;
            if (IsKeyDown(KEY_W))
                cameraPosition.y += 30 * deltaTime;
            if (IsKeyDown(KEY_S))
                cameraPosition.y -= 30 * deltaTime;
            if (IsKeyDown(KEY_Q))
                cameraPosition.z -= 30 * deltaTime;
            if (IsKeyDown(KEY_E))
                cameraPosition.z += 30 * deltaTime;
        }
        skipFrame = !skipFrame;
#endif

        // // Set up rotation matrices
        // theta += 0.5f * deltaTime;

        Matrix4 rotationMatrixZ, rotationMatrixX;
        // Rotation Z
        rotationMatrixZ = matrixMakeRotationZ(theta * 0.5f);
        rotationMatrixX = matrixMakeRotationX(theta);

        Matrix4 translationMatrix = matrixMakeTranslation(0.0f, 0.0f, 0.5f);

        Matrix4 worldMatrix = matrixMakeIdentity();
        worldMatrix = matrixMultiplyMatrix(rotationMatrixZ, rotationMatrixX);
        worldMatrix = matrixMultiplyMatrix(worldMatrix, translationMatrix);

        Vector4 cameraUp = {0.0f, 1.0f, 0.0f};
        Vector4 cameraTarget = {0.0f, 0.0f, 1.0f};
        Matrix4 cameraRotationMatrix = matrixMakeRotationY(yaw);
        cameraLookDirection = matrixMultiplyVector(cameraRotationMatrix, cameraTarget);
        cameraTarget = Vector_Add(cameraPosition, cameraLookDirection);
        Matrix4 cameraMatrix = matrixPointAt(cameraPosition, cameraTarget, cameraUp);

        Matrix4 viewMatrix = matrixQuickInverse(cameraMatrix);

        // printf("S[%f, %f, %f, %f]\n", viewMatrix.m[0][0], viewMatrix.m[0][1], viewMatrix.m[0][2], viewMatrix.m[0][3]);
        // printf("[%f, %f, %f, %f]\n", viewMatrix.m[1][0], viewMatrix.m[1][1], viewMatrix.m[1][2], viewMatrix.m[1][3]);
        // printf("[%f, %f, %f, %f]\n", viewMatrix.m[2][0], viewMatrix.m[2][1], viewMatrix.m[2][2], viewMatrix.m[2][3]);
        // printf("[%f, %f, %f, %f]E\n", viewMatrix.m[3][0], viewMatrix.m[3][1], viewMatrix.m[3][2], viewMatrix.m[3][3]);

        std::vector<Triangle> trianglesToRender;

        for (auto tri : cubeMesh.tris)
        {
            Triangle projectedTriangle, transformedTriangle, viewedTriangle;

            transformedTriangle.vertices[0] = matrixMultiplyVector(worldMatrix, tri.vertices[0]);
            transformedTriangle.vertices[1] = matrixMultiplyVector(worldMatrix, tri.vertices[1]);
            transformedTriangle.vertices[2] = matrixMultiplyVector(worldMatrix, tri.vertices[2]);

            Vector4 normal, line1, line2;
            line1 = Vector_Sub(transformedTriangle.vertices[1], transformedTriangle.vertices[0]);
            line2 = Vector_Sub(transformedTriangle.vertices[2], transformedTriangle.vertices[0]);

            normal = Vector_CrossProduct(line1, line2);

            // It's normally normal to normalise the normal
            normal = Vector_Normalise(normal);

            Vector4 cameraRay = Vector_Sub(transformedTriangle.vertices[0], cameraPosition);

            if (Vector_DotProduct(normal, cameraRay) < 0.0f)
            {

                Vector4 lightDirection = {0.0f, 1.0f, -1.0f};

                lightDirection = Vector_Normalise(lightDirection);

// How similar is normal to light direction
#ifdef ZYNGINE_ESP32S3
                float dp = max(0.1f, Vector_DotProduct(normal, lightDirection));
#endif
#ifndef ZYNGINE_ES32S3
                float dp = std::max(0.1f, Vector_DotProduct(normal, lightDirection));
#endif

                uint16_t finalColor = RGB888ToRGB565(255, 255, 255, dp);

                viewedTriangle.vertices[0] = matrixMultiplyVector(viewMatrix, transformedTriangle.vertices[0]);
                viewedTriangle.vertices[1] = matrixMultiplyVector(viewMatrix, transformedTriangle.vertices[1]);
                viewedTriangle.vertices[2] = matrixMultiplyVector(viewMatrix, transformedTriangle.vertices[2]);
                viewedTriangle.color;

                int noClippedTriangles = 0;
                Triangle clipped[2];
                noClippedTriangles = triangleClipAgainstPlane({0.0f, 0.0f, 0.1f}, {0.0f, 0.0f, 1.0f}, viewedTriangle, clipped[0], clipped[1]);

                for (int n = 0; n < noClippedTriangles; n++)
                {

                    projectedTriangle.vertices[0] = matrixMultiplyVector(projectionMatrix, clipped[n].vertices[0]);
                    projectedTriangle.vertices[1] = matrixMultiplyVector(projectionMatrix, clipped[n].vertices[1]);
                    projectedTriangle.vertices[2] = matrixMultiplyVector(projectionMatrix, clipped[n].vertices[2]);
                    projectedTriangle.color = finalColor;

                    projectedTriangle.vertices[0] = Vector_Div(projectedTriangle.vertices[0], projectedTriangle.vertices[0].w);
                    projectedTriangle.vertices[1] = Vector_Div(projectedTriangle.vertices[1], projectedTriangle.vertices[1].w);
                    projectedTriangle.vertices[2] = Vector_Div(projectedTriangle.vertices[2], projectedTriangle.vertices[2].w);

                    projectedTriangle.vertices[0].x *= -1.0f;
                    projectedTriangle.vertices[1].x *= -1.0f;
                    projectedTriangle.vertices[2].x *= -1.0f;

                    projectedTriangle.vertices[0].y *= -1.0f;
                    projectedTriangle.vertices[1].y *= -1.0f;
                    projectedTriangle.vertices[2].y *= -1.0f;

                    Vector4 offsetView = {1, 1, 0};
                    projectedTriangle.vertices[0] = Vector_Add(projectedTriangle.vertices[0], offsetView);
                    projectedTriangle.vertices[1] = Vector_Add(projectedTriangle.vertices[1], offsetView);
                    projectedTriangle.vertices[2] = Vector_Add(projectedTriangle.vertices[2], offsetView);

                    projectedTriangle.vertices[0].x *= 0.5f * (float)screenWidth;
                    projectedTriangle.vertices[0].y *= 0.5f * (float)screenHeight;
                    projectedTriangle.vertices[1].x *= 0.5f * (float)screenWidth;
                    projectedTriangle.vertices[1].y *= 0.5f * (float)screenHeight;
                    projectedTriangle.vertices[2].x *= 0.5f * (float)screenWidth;
                    projectedTriangle.vertices[2].y *= 0.5f * (float)screenHeight;
                    trianglesToRender.push_back(projectedTriangle);
                }
            }
        }
        // Sort triangles from back to front
        sort(trianglesToRender.begin(), trianglesToRender.end(), [](Triangle &t1, Triangle &t2)
             {
			float z1 = (t1.vertices[0].z + t1.vertices[1].z + t1.vertices[2].z) / 3.0f;
			float z2 = (t2.vertices[0].z + t2.vertices[1].z + t2.vertices[2].z) / 3.0f;
			return z1 > z2; });

        // Clear the screen and draw the triangles
        renderer->clear();
        sprintf(fps, "%.2f Axis: %d , %f, %f, %f", (1.0f / deltaTime), toggle, cameraPosition.x, cameraPosition.y, cameraPosition.z);

        renderer->printText(20, 20, fps, 0x0000, 0xFFFF);

        for (auto &triangle : trianglesToRender)
        {
            renderer->fillTriangle(triangle.vertices[0].x, triangle.vertices[0].y, triangle.vertices[1].x, triangle.vertices[1].y, triangle.vertices[2].x, triangle.vertices[2].y, triangle.color);
            // renderer->drawTriangle(triangle.vertices[0].x, triangle.vertices[0].y, triangle.vertices[1].x, triangle.vertices[1].y, triangle.vertices[2].x, triangle.vertices[2].y, TFT_BLACK);
        }

        // Loop through all transformed, viewed, projected, and sorted triangles
        for (auto &triToRaster : trianglesToRender)
        {
            // Clip triangles against all four screen edges, this could yield
            // a bunch of triangles, so create a queue that we traverse to
            //  ensure we only test new triangles generated against planes
            Triangle clipped[2];
            std::list<Triangle> listTriangles;

            // Add initial triangle
            listTriangles.push_back(triToRaster);
            int nNewTriangles = 1;

            for (int p = 0; p < 4; p++)
            {
                int nTrisToAdd = 0;
                while (nNewTriangles > 0)
                {
                    // Take triangle from front of queue
                    Triangle test = listTriangles.front();
                    listTriangles.pop_front();
                    nNewTriangles--;

                    // Clip it against a plane. We only need to test each
                    // subsequent plane, against subsequent new triangles
                    // as all triangles after a plane clip are guaranteed
                    // to lie on the inside of the plane. I like how this
                    // comment is almost completely and utterly justified
                    switch (p)
                    {
                    case 0:
                        nTrisToAdd = triangleClipAgainstPlane({0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, test, clipped[0], clipped[1]);
                        break;
                    case 1:
                        nTrisToAdd = triangleClipAgainstPlane({0.0f, (float)screenHeight - 1, 0.0f}, {0.0f, -1.0f, 0.0f}, test, clipped[0], clipped[1]);
                        break;
                    case 2:
                        nTrisToAdd = triangleClipAgainstPlane({0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, test, clipped[0], clipped[1]);
                        break;
                    case 3:
                        nTrisToAdd = triangleClipAgainstPlane({(float)screenWidth - 1, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, test, clipped[0], clipped[1]);
                        break;
                    }

                    // Clipping may yield a variable number of triangles, so
                    // add these new ones to the back of the queue for subsequent
                    // clipping against next planes
                    for (int w = 0; w < nTrisToAdd; w++)
                        listTriangles.push_back(clipped[w]);
                }
                nNewTriangles = listTriangles.size();
            }

            // Draw the transformed, viewed, clipped, projected, sorted, clipped triangles
            for (auto &t : listTriangles)
            {
                // printf("(%d, %d), (%d, %d), (%d, %d)", t.vertices[0].x, t.vertices[0].y, t.vertices[1].x, t.vertices[1].y, t.vertices[2].x, t.vertices[2].y);
                renderer->fillTriangle(t.vertices[0].x, t.vertices[0].y, t.vertices[1].x, t.vertices[1].y, t.vertices[2].x, t.vertices[2].y, t.color);
                // DrawTriangle(t.vertices[0].x, t.vertices[0].y, t.vertices[1].x, t.vertices[1].y, t.vertices[2].x, t.vertices[2].y, PIXEL_SOLID, FG_BLACK);
            }
        }
    }
};

MyGame game;

#ifdef ZYNGINE_ESP32S3
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
#endif

#ifdef ZYNGINE_WINDOWS_NATIVE_RAYLIB_CUSTOM_SOFTWARE_RENDERER
int main()
{
    printf("Hello World");
    if (game.initialize(320, 480))
    {
        game.run();
    }
}
#endif