#include <zyngine.h>
#include <zynmesh.h>
#include <zynlight.h>
#include <vector>

class Test : public Zyngine
{
private:
    ZMesh model;
    ZynLight light;

public:
    void onUserCreate() override
    {
        model.loadFromObjectFile("./resources/optimized_assets/3d_models/african_head.obj");
        light;
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
                renderer->renderTriangle(pts, getIntensityRGB565(intensity, ZYN_WHITE));
            }
        }

        // for (int i = 0; i < model.tris.size(); i++)
        // {
        //     ZTriangle triangle = model.tris[i];
        //     ZVec2i screen_coords[3];
        //     ZVec3 world_coords[3];
        //     for (int j = 0; j < 3; j++)
        //     {
        //         ZVec3 v = triangle.v[j];
        //         screen_coords[j] = ZVec2i((v.x + 1.) * screenWidth / 2., (v.y + 1.) * screenHeight / 2.);
        //         world_coords[j] = v;
        //     }

        //     ZVec3 n = (world_coords[2].sub(world_coords[0])).cross(world_coords[1].sub(world_coords[0]));

        //     n.normalize();
        //     // TODO check dot function
        //     float intensity = light_dir.dot(n);

        //     if (intensity > 0)
        //     {
        //         renderer->renderTriangle(screen_coords, getIntensityRGB565(intensity, ZYN_WHITE));
        //     }
        // }

        // renderer->drawTriangle(12, 12, 40, 40, 30, 30, rand() % 65535);
    }
};

Test engine;

int main()
{
    if (engine.initialize(320, 480, 60))
        engine.run();
}