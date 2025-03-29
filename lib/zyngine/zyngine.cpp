#include <zyngine.h>

bool Zyngine::initialize(int width, int height, int targetFPS)
{
    // Implementation for initializing the engine
    // Example: Set up the renderer, input, etc.
    screenWidth = width;
    screenHeight = height;
    onUserCreate();

#ifdef ZYNGINE_ESP32S3
    renderer = new ZynRenderer(screenWidth, screenHeight, new ParallelILI9486());
    start_time = millis();
    targetFrameTime = 1.0f / (float)targetFPS;
#endif

#ifdef ZYNGINE_NATIVE_RAYLIB
    renderer = new ZynRenderer(screenWidth, screenHeight);
#endif

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

#ifdef ZYNGINE_NATIVE_RAYLIB
    while (!WindowShouldClose())
    {
        BeginDrawing();
        onUserUpdate(GetFrameTime());
        EndDrawing();
    }
#endif
}