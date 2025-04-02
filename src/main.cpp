#include <zyngine.h>
#include <zynmodel.h>
#include <zynlight.h>
#include <zyncamera.h>
#include <zyntexture.h>
#include <vector>

class Test : public Zyngine
{
private:
    ZModel model;
    ZynTexture texture;
    ZynLight light;
    ZynLight lightFixed;
    ZynCamera camera;
    ZMat4 modelViewMatrix;
    ZMat4 projectionMatrix;
    ZMat4 viewPortMatrix;
    ZMat4 translationMat;
    ZMat4 z;
    int depth = 255;

public:
    void onUserCreate() override
    {
        lightFixed.l = ZVec3(-1.0f, -1.0f, 0.0f);
        modelViewMatrix = camera.getLookAtMatrix();
        projectionMatrix;
        viewPortMatrix.toViewport(screenWidth / 8, screenHeight / 8, screenWidth * 3 / 4, screenHeight * 3 / 4, depth);
        projectionMatrix.m32 = -1.0f / (camera.eye.sub(camera.center)).normalize();

        model.loadModel("test", "./resources/optimized_assets/3d_models");

        z = viewPortMatrix.mulMatrix(projectionMatrix.mulMatrix(modelViewMatrix));
    }

    void onUserUpdate(float deltaTime) override
    {
        // Handle keyboard input for model movement
        ZVec3 translation(0.0f, 0.0f, 0.0f);
        float moveSpeed = 5.0f * deltaTime;

        if (IsKeyDown(KEY_W))
            translation.y += moveSpeed; // Move up
        if (IsKeyDown(KEY_S))
            translation.y -= moveSpeed; // Move down
        if (IsKeyDown(KEY_A))
            translation.x -= moveSpeed; // Move left
        if (IsKeyDown(KEY_D))
            translation.x += moveSpeed; // Move right
        if (IsKeyDown(KEY_Q))
            translation.z -= moveSpeed; // Move forward
        if (IsKeyDown(KEY_E))
            translation.z += moveSpeed; // Move backward

        // Update the translation matrix
        translationMat = translationMat.translate(translation.x, translation.y, translation.z);

        ZVec3 rotate(0.0f, 0.0f, 0.0f);

        if (IsKeyDown(KEY_UP))
            rotate.y += moveSpeed; // Move up
        if (IsKeyDown(KEY_DOWN))
            rotate.y -= moveSpeed; // Move down
        if (IsKeyDown(KEY_LEFT))
            rotate.x -= moveSpeed; // Move left
        if (IsKeyDown(KEY_RIGHT))
            rotate.x += moveSpeed; // Move right
        if (IsKeyDown(KEY_PERIOD))
            rotate.z -= moveSpeed; // Move forward
        if (IsKeyDown(KEY_COMMA))
            rotate.z += moveSpeed; // Move backward

        translationMat = translationMat.rotate(rotate.x, rotate.y, rotate.z);

        light.l = light.l.normalized();
        // Display the position of the light on the screen
        ZVec3i lightScreenPos = (z.mulVector(ZVec4(light.l))).toZVec3().toZVec3i();

        renderer->clear(ZYN_BLACK);

        for (int i = 0; i < model.mesh.tris.size(); i++)
        {
            ZTriangle triangle = model.mesh.tris[i];
            ZVec3i screenCoords[3];
            ZVec3 worldCoords[3];
            float intensities[3] = {0, 0, 0};

            for (int j = 0; j < 3; j++)
            {
                ZVec4 v(triangle.v[j]);
                v = translationMat.mulVector(v);
                ZVec3 n = translationMat.mulVector(triangle.n[j]).normalized();
                screenCoords[j] = (z.mulVector(v)).toZVec3().toZVec3i();
                worldCoords[j] = triangle.v[j];

                intensities[j] += light.getIntensityAtNorm(n);
                intensities[j] += lightFixed.getIntensityAtNorm(n);
            }
            renderer->renderTexturedTriangle(screenCoords, triangle.t, intensities, &model.diffuseMap);

            renderer->renderSphere(lightScreenPos, ZYN_WHITE);
            renderer->renderSphere((z.mulVector(ZVec4(lightFixed.l))).toZVec3().toZVec3i(), ZYN_WHITE);
        }
    }
};

Test engine;

int main()
{
    if (engine.initialize(320, 480, 60))
        engine.run();
}