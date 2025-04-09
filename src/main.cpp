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
    ZModel floor;
    ZynTexture texture;
    ZynLight light;
    ZynLight lightFixed;
    ZynCamera camera;
    ZMat4 modelViewMatrix;
    ZMat4 projectionMatrix;
    ZMat4 viewPortMatrix;
    ZMat4 translationMat;
    ZMat4 z;
    float near = 0.1f;
    float far = -100.0f;
    int depth = 255;

public:
    void onUserCreate() override
    {
        light.l = ZVec3(0.0f, -0.5f, 0.1f).normalized();
        lightFixed.l = ZVec3(1.0f, 1.0f, 1.0f).normalized();
        modelViewMatrix = camera.getLookAtMatrix();
        viewPortMatrix.toViewport(0, 0, screenWidth, screenHeight, depth);
        projectionMatrix = camera.getProjectionMatrix();
        model.loadModel("test", "./resources/optimized_assets/3d_models");
        floor.loadModel("floor", "./resources/optimized_assets/3d_models");

        translationMat = translationMat.translate(1.0f, 1.0f, 0.0f);
        for (int i = 0; i < model.mesh.tris.size(); i++)
        {
            model.mesh.tris[i].v[0] = translationMat.mulVector(model.mesh.tris[i].v[0]);
            model.mesh.tris[i].v[1] = translationMat.mulVector(model.mesh.tris[i].v[1]);
            model.mesh.tris[i].v[2] = translationMat.mulVector(model.mesh.tris[i].v[2]);
        }
        translationMat.reset();

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

        ZynCamera cam = camera.copy();
        // cam.eye = camera.eye.add(translation);
        // cam.center = camera.center.add(translation);
        cam.eye = translationMat.mulVector(cam.eye);
        cam.center = translationMat.mulVector(cam.center);
        modelViewMatrix = cam.getLookAtMatrix();

        z = viewPortMatrix.mulMatrix(projectionMatrix.mulMatrix(modelViewMatrix));

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
                worldCoords[j] = v.toZVec3();
                ZVec3 n = translationMat.mulVector(triangle.n[j]).normalized();

                screenCoords[j] = (z.mulVector(v)).toZVec3().toZVec3i();

                intensities[j] += light.getIntensityAtNorm(n);
                intensities[j] += lightFixed.getIntensityAtNorm(n);
            }

            if (isInClipView(worldCoords, near, far))
                renderer->renderTexturedTriangle(screenCoords, triangle.t, intensities, &model.diffuseMap);
        }

        for (int i = 0; i < floor.mesh.tris.size(); i++)
        {
            ZTriangle triangle = floor.mesh.tris[i];
            ZVec3i screenCoords[3];
            float intensities[3] = {0, 0, 0};

            for (int j = 0; j < 3; j++)
            {
                ZVec4 v(triangle.v[j]);
                // v = translationMat.mulVector(v);
                // ZVec3 n = translationMat.mulVector(triangle.n[j]).normalized();a
                screenCoords[j] = (z.mulVector(v)).toZVec3().toZVec3i();

                intensities[j] += light.getIntensityAtNorm(triangle.n[j]);
                intensities[j] += lightFixed.getIntensityAtNorm(triangle.n[j]);
            }
            renderer->renderTexturedTriangle(screenCoords, triangle.t, intensities, &floor.diffuseMap);
        }

        ZVec3 v = z.mulVector(light.l);
        DrawCircle(v.x, v.y, v.z / 400, WHITE);
        v = z.mulVector(lightFixed.l);
        DrawCircle(v.x, v.y, v.z / 400, WHITE);
    }
};

Test engine;

int main()
{
    if (engine.initialize(320, 480, 60))
        engine.run();
}