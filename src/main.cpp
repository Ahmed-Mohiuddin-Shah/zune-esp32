#include <zyngine.h>
#include <zynmesh.h>
#include <vector>

class Test : public Zyngine
{
private:
    ZMesh model;

public:
    ZVec3 barycentric(ZVec2i *pts, ZVec2i P)
    {
        ZVec3 u = ZVec3(pts[2].x - pts[0].x, pts[1].x - pts[0].x, pts[0].x - P.x).cross(ZVec3(pts[2].y - pts[0].y, pts[1].y - pts[0].y, pts[0].y - P.y));
        /* `pts` and `P` has integer value as coordinates
           so `abs(u[2])` < 1 means `u[2]` is 0, that means
           triangle is degenerate, in this case return something with negative coordinates */
        if (std::abs(u.z) < 1)
            return ZVec3(-1, 1, 1);
        return ZVec3(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
    }

    void triangle(ZVec2i *pts, uint16_t color)
    {
        ZVec2i bboxmin(screenWidth - 1, screenHeight - 1);
        ZVec2i bboxmax(0, 0);
        ZVec2i clamp(screenWidth - 1, screenHeight - 1);
        for (int i = 0; i < 3; i++)
        {
            bboxmin.x = std::max(0, std::min(bboxmin.x, pts[i].x));
            bboxmin.y = std::max(0, std::min(bboxmin.y, pts[i].y));

            bboxmax.x = std::min(clamp.x, std::max(bboxmax.x, pts[i].x));
            bboxmax.y = std::min(clamp.y, std::max(bboxmax.y, pts[i].y));
        }
        ZVec2i P;
        for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++)
        {
            for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++)
            {
                ZVec3 bc_screen = barycentric(pts, P);
                if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0)
                    continue;
                // image.set(P.x, P.y, color);
                renderer->drawPixel(P.x, P.y, color);
            }
        }
    }

    void onUserCreate() override
    {
        model.loadFromObjectFile("./resources/optimized_assets/3d_models/african_head.obj");
    }

    void onUserUpdate(float deltaTime) override
    {
        renderer->clear(ZYN_BLACK);

        // for (int i = 0; i < model.tris.size(); i++)
        // {
        //     ZTriangle triangle = model.tris[i];
        //     for (int j = 0; j < 3; j++)
        //     {
        //         ZVec3 v0 = triangle.v[j];
        //         ZVec3 v1 = triangle.v[(j + 1) % 3];
        //         int x0 = (v0.x + 1.) * screenWidth / 2.;
        //         int y0 = (v0.y + 1.) * screenHeight / 2.;
        //         int x1 = (v1.x + 1.) * screenWidth / 2.;
        //         int y1 = (v1.y + 1.) * screenHeight / 2.;
        //         renderer->drawLine(x0, y0, x1, y1, ZYN_WHITE);
        //     }
        // }

        for (int i = 0; i < model.tris.size(); i++)
        {
            ZTriangle triangle = model.tris[i];
            ZVec2i screen_coords[3];
            for (int j = 0; j < 3; j++)
            {
                ZVec3 world_coords = triangle.v[j];
                screen_coords[j] = ZVec2i((world_coords.x + 1.) * screenWidth / 2., (world_coords.y + 1.) * screenHeight / 2.);
            }
            this->triangle(screen_coords, rand() % 65535);
        }

        renderer->drawTriangle(12, 12, 40, 40, 30, 30, rand() % 65535);
    }
};

Test engine;

int main()
{
    if (engine.initialize(800, 600, 60))
        engine.run();
}