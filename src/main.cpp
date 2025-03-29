#include <zyngine.h>

class Test : public Zyngine
{
private:
    /* data */
public:
    void onUserCreate() override
    {
    }

    void onUserUpdate(float deltaTime) override
    {
        renderer->clear(ZYN_BLACK);
        renderer->drawTriangle(12, 12, 40, 40, 30, 30, ZYN_WHITE);
    }
};

Test engine;

int main()
{
    if (engine.initialize(320, 480, 60))
        engine.run();
}