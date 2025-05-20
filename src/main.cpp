#include <zyngine.h>

class WaterSim : public Zyngine
{
protected:
    void onUserCreate() override
    {
        Serial.begin(115200);
    }

    void onUserUpdate(float deltaTime) override
    {
        renderer->clear(ZYN_BLACK);
    }
};

WaterSim waterSim;

void setup()
{
    waterSim.initialize(320, 480, 60);
}

void loop()
{
    waterSim.run();
}

#ifdef ZYNGINE_NATIVE_RAYLIB
int main()
{
    setup();
    loop();
}
#endif