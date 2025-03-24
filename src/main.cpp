#include <zyngine.h>
#include <zutils.h>
#include <zyncolor.h>

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
#include <iostream>

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
        if (!cubeMesh.loadFromObjectFile("./resources/optimized_assets/3d_models/texture_test.obj", true))
        {
#ifdef ZYNGINE_ESP32S3
            Serial.println("Failed to load cube mesh");
            return;
#endif
        }

        std::cout << "Cube Mesh Information:" << std::endl;
        std::cout << "Number of triangles: " << cubeMesh.tris.size() << std::endl;

        for (size_t i = 0; i < cubeMesh.tris.size(); ++i)
        {
            const auto &tri = cubeMesh.tris[i];
            std::cout << "Triangle " << i << ":" << std::endl;
            for (int j = 0; j < 3; ++j)
            {
                std::cout << "  Vertex " << j << ": ("
                          << tri.vertices[j].x << ", "
                          << tri.vertices[j].y << ", "
                          << tri.vertices[j].z << ")" << std::endl;
            }
            for (int j = 0; j < 3; ++j)
            {
                std::cout << "  UVs " << j << ": ("
                          << tri.texture[j].x << ", "
                          << tri.texture[j].y << ", "
                          << tri.texture[j].z << ")" << std::endl;
            }
            std::cout << "  Material Index: " << tri.materialIndex << std::endl;
            std::cout << "  Texture Size: " << cubeMesh.textures[tri.materialIndex].width << "x" << cubeMesh.textures[tri.materialIndex].height << std::endl;
        }

        for (size_t i = 0; i < 10; ++i)
        {
            const auto &mat = cubeMesh.materials[i];
            std::cout << "Material " << i << ":" << std::endl;
            std::cout << "  Ambient: " << mat.Ka << std::endl;
            std::cout << "  Diffuse: " << mat.Kd << std::endl;
            std::cout << "  Specular: " << mat.Ks << std::endl;
            std::cout << "  Shininess: " << mat.Ns << std::endl;
        }

        std::cout << "file Loaded" << std::endl;

        projectionMatrix = createProjectionMatrix(screenWidth, screenHeight, 60.0f, 0.1f, 100.0f);

        cameraPosition = {30.0f, 20.0f, -90.0f};

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

        // renderer->printText(0, 0, "Hello World", ZYN_BLACK, ZYN_WHITE);

        // return;
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

        std::vector<Triangle> trianglesToRender;

        for (auto tri : cubeMesh.tris)
        {
            Triangle projectedTriangle, transformedTriangle, viewedTriangle;

            transformedTriangle.vertices[0] = matrixMultiplyVector(worldMatrix, tri.vertices[0]);
            transformedTriangle.vertices[1] = matrixMultiplyVector(worldMatrix, tri.vertices[1]);
            transformedTriangle.vertices[2] = matrixMultiplyVector(worldMatrix, tri.vertices[2]);
            transformedTriangle.texture[0] = tri.texture[0];
            transformedTriangle.texture[1] = tri.texture[1];
            transformedTriangle.texture[2] = tri.texture[2];

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
                viewedTriangle.texture[0] = transformedTriangle.texture[0];
                viewedTriangle.texture[1] = transformedTriangle.texture[1];
                viewedTriangle.texture[2] = transformedTriangle.texture[2];

                int noClippedTriangles = 0;
                Triangle clipped[2];
                noClippedTriangles = triangleClipAgainstPlane({0.0f, 0.0f, 0.1f}, {0.0f, 0.0f, 1.0f}, viewedTriangle, clipped[0], clipped[1]);

                for (int n = 0; n < noClippedTriangles; n++)
                {

                    projectedTriangle.vertices[0] = matrixMultiplyVector(projectionMatrix, clipped[n].vertices[0]);
                    projectedTriangle.vertices[1] = matrixMultiplyVector(projectionMatrix, clipped[n].vertices[1]);
                    projectedTriangle.vertices[2] = matrixMultiplyVector(projectionMatrix, clipped[n].vertices[2]);
                    projectedTriangle.color = finalColor;
                    projectedTriangle.texture[0] = clipped[n].texture[0];
                    projectedTriangle.texture[1] = clipped[n].texture[1];
                    projectedTriangle.texture[2] = clipped[n].texture[2];

                    projectedTriangle.texture[0].x = projectedTriangle.texture[0].x / projectedTriangle.vertices[0].w;
                    projectedTriangle.texture[1].x = projectedTriangle.texture[1].x / projectedTriangle.vertices[1].w;
                    projectedTriangle.texture[2].x = projectedTriangle.texture[2].x / projectedTriangle.vertices[2].w;

                    projectedTriangle.texture[0].y = projectedTriangle.texture[0].y / projectedTriangle.vertices[0].w;
                    projectedTriangle.texture[1].y = projectedTriangle.texture[1].y / projectedTriangle.vertices[1].w;
                    projectedTriangle.texture[2].y = projectedTriangle.texture[2].y / projectedTriangle.vertices[2].w;

                    projectedTriangle.texture[0].z = 1.0f / projectedTriangle.vertices[0].w;
                    projectedTriangle.texture[1].z = 1.0f / projectedTriangle.vertices[1].w;
                    projectedTriangle.texture[2].z = 1.0f / projectedTriangle.vertices[2].w;

                    // Scale into view, we moved the normalising into cartesian space
                    // out of the matrix.vector function from the previous videos, so
                    // do this manually
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

        // // Sort triangles from back to front
        // sort(trianglesToRender.begin(), trianglesToRender.end(), [](Triangle &t1, Triangle &t2)
        //      {
        // 	float z1 = (t1.vertices[0].z + t1.vertices[1].z + t1.vertices[2].z) / 3.0f;
        // 	float z2 = (t2.vertices[0].z + t2.vertices[1].z + t2.vertices[2].z) / 3.0f;
        // 	return z1 > z2; });

        // Clear the screen and draw the triangles
        renderer->clear();
        sprintf(fps, "%.2f Axis: %d , %f, %f, %f", (1.0f / deltaTime), toggle, cameraPosition.x, cameraPosition.y, cameraPosition.z);

        renderer->printText(20, 20, fps, 0x0000, 0xFFFF);

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
                renderer->drawTexturedTriangle(t.vertices[0].x, t.vertices[0].y, t.texture[0].x, t.texture[0].y, t.texture[0].z,
                                 t.vertices[1].x, t.vertices[1].y, t.texture[1].x, t.texture[1].y, t.texture[1].z,
                                 t.vertices[2].x, t.vertices[2].y, t.texture[2].x, t.texture[2].y, t.texture[2].z, cubeMesh.materials[t.materialIndex].texture);

            renderer->fillTriangle(t.vertices[0].x, t.vertices[0].y, t.vertices[1].x, t.vertices[1].y, t.vertices[2].x, t.vertices[2].y, t.color);
            renderer->drawTriangle(t.vertices[0].x, t.vertices[0].y, t.vertices[1].x, t.vertices[1].y, t.vertices[2].x, t.vertices[2].y, 0xFFFF);
            }
        }

        // for (auto &triangle : trianglesToRender)
        // {
        //     // printf("Triangle vertices: \n");
        //     // printf("V1: (%f, %f, %f)\n", triangle.vertices[0].x, triangle.vertices[0].y, triangle.vertices[0].z);
        //     // printf("V2: (%f, %f, %f)\n", triangle.vertices[1].x, triangle.vertices[1].y, triangle.vertices[1].z);
        //     // printf("V3: (%f, %f, %f)\n", triangle.vertices[2].x, triangle.vertices[2].y, triangle.vertices[2].z);
        //     // printf("Material info: \n");
        //     // printf("Ambient: (%f)\n", triangle.mat.Ka);
        //     // printf("Diffuse: (%f)\n", triangle.mat.Kd);
        //     // printf("Specular: (%f)\n", triangle.mat.Ks);
        //     // printf("Shininess: %f\n", triangle.mat.Ns);
        //     // printf("W, H : %d, %d\n", triangle.mat.texture->width, triangle.mat.texture->height);
        //     // std::cout << triangle.texture[0].x << ", " << triangle.texture[0].y << ", " << triangle.texture[0].z << std::endl;
        //     // std::cout << triangle.texture[1].x << ", " << triangle.texture[1].y << ", " << triangle.texture[1].z << std::endl;
        //     // std::cout << triangle.texture[2].x << ", " << triangle.texture[2].y << ", " << triangle.texture[2].z << std::endl;

        //     // renderer->drawTexture(0, 0, triangle.mat.texture);
        //     // renderer->drawTexturedTriangle(
        //     //     triangle.vertices[0].x, triangle.vertices[0].y, triangle.texture[0].x, triangle.texture[0].y, triangle.texture[0].z,
        //     //     triangle.vertices[1].x, triangle.vertices[1].y, triangle.texture[1].x, triangle.texture[1].y, triangle.texture[1].z,
        //     //     triangle.vertices[2].x, triangle.vertices[2].y, triangle.texture[2].x, triangle.texture[2].y, triangle.texture[2].z,
        //     //     (cubeMesh.materials[triangle.materialIndex].texture));
        //     // renderer->fillTriangle(triangle.vertices[0].x, triangle.vertices[0].y, triangle.vertices[1].x, triangle.vertices[1].y, triangle.vertices[2].x, triangle.vertices[2].y, triangle.color);
        //     renderer->drawTriangle(triangle.vertices[0].x, triangle.vertices[0].y, triangle.vertices[1].x, triangle.vertices[1].y, triangle.vertices[2].x, triangle.vertices[2].y, 0xFFFF);
        // }
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
    std::cout << "Hello World";
    if (game.initialize(320, 480))
    {
        game.run();
    }
}
#endif