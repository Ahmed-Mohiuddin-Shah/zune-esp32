#include "zyngine.h"
#include "devicedrivers.h"

bool Zyngine::initialize(int width, int height, int targetFPS)
{
    // Implementation for initializing the engine
    // Example: Set up the renderer, input, etc.
    screenWidth = width;
    screenHeight = height;

#ifdef ZYNGINE_ESP32S3
    renderer = new ZynRenderer(screenWidth, screenHeight, new ParallelILI9486());
    start_time = millis();
    targetFrameTime = 1.0f / (float)targetFPS;
#endif

#ifdef ZYNGINE_WINDOWS_NATIVE_RAYLIB_CUSTOM_SOFTWARE_RENDERER
    renderer = new ZynRenderer(screenWidth, screenHeight);
#endif

    onUserCreate();
    return true;
}

void Zyngine::run()
{

#ifdef ZYNGINE_ESP32S3
    while (true)
    {
        unsigned long current_time = millis();
        float deltaTime = (float)(current_time - start_time) / 1000.0f;
        start_time = current_time;

        renderer->diffDraw();
    }
#endif

#ifdef ZYNGINE_WINDOWS_NATIVE_RAYLIB_CUSTOM_SOFTWARE_RENDERER
    while(!WindowShouldClose()) {
        BeginDrawing();
        onUserUpdate(GetFrameTime());
        EndDrawing();
    }
#endif
}