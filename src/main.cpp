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
    ZynCamera camera;
    ZMat4 projection;
    ZMat4 viewPort;

public:
    void onUserCreate() override
    {

        viewPort.toViewport(screenWidth / 8, screenHeight / 8, screenWidth * 3 / 4, screenHeight * 3 / 4, 255);
        projection.m32 = -1.0f / camera.c.z;
        
        model.loadFromObjectFile("./resources/optimized_assets/3d_models/african_head.obj");
        texture.loadFromFile("./resources/optimized_assets/3d_models/textures/african_head_diffuse.zyntex");
    }

    void onUserUpdate(float deltaTime) override
    {
        renderer->clear(ZYN_BLACK);

        for (int i = 0; i < model.tris.size(); i++)
        {
            ZTriangle triangle = model.tris[i];
            ZVec3 pts[3];

            for (int j = 0; j < 3; j++)
            {
                pts[j] = renderer->world2screen(triangle.v[j]);
            }

            float intensity = light.getIntensityAtTriangle(triangle);

            if (intensity > 0)
            {
                renderer->renderTexturedTriangle(pts, triangle.t, intensity, &texture);
            }
        }

        // renderer->drawTriangle(12, 12, 40, 40, 30, 30, rand() % 65535);
    }
};

Test engine;

int main()
{
    if (engine.initialize(320, 480, 60))
        engine.run();
}