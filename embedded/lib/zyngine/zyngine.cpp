#include "zyngine.h"
#include "devicedrivers.h"

bool Zyngine::initialize(int width, int height, int targetFPS)
{
    // Implementation for initializing the engine
    // Example: Set up the renderer, input, etc.
    screenWidth = width;
    screenHeight = height;
    renderer = new ZynRenderer(screenWidth, screenHeight, new ParallelILI9486());
    start_time = millis();
    targetFrameTime = 1.0f / (float)targetFPS;
    onUserCreate();
    return true;
}

void Zyngine::run()
{

    while (true)
    {
        unsigned long current_time = millis();
        float deltaTime = (float)(current_time - start_time) / 1000.0f;
        start_time = current_time;

        onUserUpdate(deltaTime);

        renderer->diffDraw();
    }
}