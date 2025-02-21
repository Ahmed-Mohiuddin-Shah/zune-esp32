#include <raylib/raylib.h>
#include "config.h"

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "ZUNE ESP32-S3");
    // TTF font : Font data and atlas are generated directly from TTF
    // NOTE: We define a font base size of 32 pixels tall and up-to 250 characters
    Font font = LoadFontEx("resources/droidsans.ttf", 100, 0, 250);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(backgroundColor);
        DrawTextEx(font, "Congrats! You created your first window!", {190, 200}, 50, 16, LIGHTGRAY);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}