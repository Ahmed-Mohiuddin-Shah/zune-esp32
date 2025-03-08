#include "Zyngine.h"

bool Zyngine::initialize(int width, int height, int targetFPS)
{
    // Implementation for initializing the engine
    // Example: Set up the renderer, input, etc.
    screenWidth = width;
    screenHeight = height;
    renderer = new ZynRenderer(new TFT_eSPI(), screenWidth, screenHeight);
    start_time = millis();
    targetFrameTime = 1.0f / (float)targetFPS;
    onUserCreate();
    return true;
}

void Zyngine::run()
{
    const float targetFrameTime = 1.0f / 60.0f; // Target frame time for 60 FPS

    while (true)
    {
        unsigned long current_time = millis();
        float deltaTime = (float)(current_time - start_time) / 1000.0f;
        start_time = current_time;

        onUserUpdate(deltaTime);
    }
}