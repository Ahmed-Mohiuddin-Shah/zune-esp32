#include <zyngine.h>
#include <zynmesh.h>
#include <zynlight.h>
#include <zyncamera.h>
#include <zyntexture.h>
#include <vector>

class Test : public Zyngine
{
private:
    ZMesh model;
    ZynTexture texture;
    ZynLight light;
    ZynLight lightFixed;
    ZynCamera camera;
    ZMat4 modelViewMatrix;
    ZMat4 projectionMatrix;
    ZMat4 viewPortMatrix;
    ZMat4 translationMat;
    int depth = 255;

public:
    void onUserCreate() override
    {

        modelViewMatrix = camera.getLookAtMatrix();
        projectionMatrix;
        viewPortMatrix.toViewport(screenWidth / 8, screenHeight / 8, screenWidth * 3 / 4, screenHeight * 3 / 4, depth);
        projectionMatrix.m32 = -1.0f / (camera.eye.sub(camera.center)).normalize();

        model.loadFromObjectFile("./resources/optimized_assets/3d_models/african_head.obj");
        texture.loadFromFile("./resources/optimized_assets/3d_models/textures/african_head_diffuse.zyntex");
    }

    void onUserUpdate(float deltaTime) override
    {

        float cameraSpeed = 5.0f * deltaTime;

        // Move up (Q) and down (E)
        if (IsKeyDown(KEY_Q))
        {
            light.l = light.l.add(ZVec3(0, 0, -cameraSpeed));
        }
        if (IsKeyDown(KEY_E))
        {
            light.l = light.l.add(ZVec3(0, 0, cameraSpeed));
        }

        if (IsKeyDown(KEY_S))
        {
            light.l = light.l.add(ZVec3(0, -cameraSpeed, 0));
        }
        if (IsKeyDown(KEY_W))
        {
            light.l = light.l.add(ZVec3(0, cameraSpeed, 0));
        }

        if (IsKeyDown(KEY_A))
        {
            light.l = light.l.add(ZVec3(-cameraSpeed, 0, 0));
        }
        if (IsKeyDown(KEY_D))
        {
            light.l = light.l.add(ZVec3(cameraSpeed, 0, 0));
        }

        light.l = light.l.normalized();
        // Display the position of the light on the screen
        ZVec3i lightScreenPos = (viewPortMatrix.mulVector(projectionMatrix.mulVector(ZVec4(light.l)))).toZVec3().toZVec3i();

        renderer->clear(ZYN_BLACK);

        for (int i = 0; i < model.tris.size(); i++)
        {
            ZTriangle triangle = model.tris[i];
            ZVec3i screenCoords[3];
            ZVec3 worldCoords[3];
            float intensities[3] = {0, 0, 0};

            for (int j = 0; j < 3; j++)
            {
                ZVec4 v(triangle.v[j]);
                ZVec3 n = triangle.n[j];
                screenCoords[j] = (viewPortMatrix.mulVector(projectionMatrix.mulVector(v))).toZVec3().toZVec3i();
                worldCoords[j] = triangle.v[j];
                intensities[j] += light.getIntensityAtNorm(n);
                intensities[j] += lightFixed.getIntensityAtNorm(n);
            }
            renderer->renderTexturedTriangle(screenCoords, triangle.t, intensities, &texture);

            renderer->renderSphere(lightScreenPos, ZYN_WHITE);
            renderer->renderSphere((viewPortMatrix.mulVector(projectionMatrix.mulVector(ZVec4(lightFixed.l)))).toZVec3().toZVec3i(), ZYN_WHITE);
        }
    }
};

Test engine;

int main()
{
    if (engine.initialize(320, 480, 60))
        engine.run();
}