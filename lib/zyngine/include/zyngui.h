#ifndef ZYNGUI_H
#define ZYNGUI_H

#include "zynrenderer.h"
#include "zynmath.h"

class ZynGUI
{
private:
    ZynRenderer *renderer;
    ZynTexture wallpaper;

    ZVec2i cursorPos;

    int screenWidth;
    int screenHeight;
    int lockScreenSlidePosition = 0;

public:
    void passRenderer(ZynRenderer *renderer)
    {
        this->renderer = renderer;
        screenWidth = renderer->getScreenWidth();
        screenHeight = renderer->getScreenHeight();
        wallpaper.loadFromFile("/resources/optimized_assets/wallpapers/Bluey Shadownlands Wallpaper (Mobile).zyntex");
    }
    // // Helper to interpolate between two colors
    // Color lerpColor(Color a, Color b, float t)
    // {
    //     return {
    //         (unsigned char)(a.r + (b.r - a.r) * t),
    //         (unsigned char)(a.g + (b.g - a.g) * t),
    //         (unsigned char)(a.b + (b.b - a.b) * t),
    //         (unsigned char)(a.a + (b.a - a.a) * t)};
    // }

    // // Draws a vertical gradient using drawLine and any number of colors
    // void drawVerticalGradientLines(int x, int y, int width, int height, const std::vector<Color> &colors)
    // {
    //     int colorCount = colors.size();
    //     if (colorCount < 2)
    //         return;

    //     if (height == 1)
    //     {
    //         // Special case: just draw a single line with the first color
    //         DrawLine(x, y, x + width - 1, y, colors[0]);
    //         return;
    //     }
    //     for (int i = 0; i < height; ++i)
    //     {
    //         float pos = (float)i / (height - 1);
    //         float scaled = pos * (colorCount - 1);
    //         int idx = (int)scaled;
    //         float localT = scaled - idx;

    //         Color c1 = colors[idx];
    //         Color c2 = colors[idx + 1 < colorCount ? idx + 1 : idx];
    //         Color c = LerpColor(c1, c2, localT);

    //         DrawLine(x, y + i, x + width - 1, y + i, c);
    //     }
    // }

    // void drawLoadingBar(int x, int y, int width, int height, float percent, const std::vector<Color> &colors, Color backgroundColor)
    // {
    //     // Clamp percent between 0 and 100
    //     if (percent < 0.0f)
    //         percent = 0.0f;
    //     if (percent > 100.0f)
    //         percent = 100.0f;

    //     // Draw the gradient background
    //     drawVerticalGradientLines(x, y, width, height, colors);

    //     // Calculate the filled bar height
    //     int filledHeight = (int)(height * (percent / 100.0f));
    //     int unfilledHeight = height - filledHeight;

    //     // Draw the unfilled (background) part only if needed
    //     if (unfilledHeight > 0)
    //     {
    //         DrawRectangle(x, y - 1, width, unfilledHeight, backgroundColor);
    //     }
    // }

    //
    void drawBatteryStatus(int x, int y, int height, float percent, int status)
    {

        uint16_t color = ZYN_RED;
        if (status == 0)
        {
            color = ZYN_RED;
        }
        else if (status == 2)
        {
            color = ZYN_WHITE;
        }
        else
        {
            color = ZYN_GREEN;
        }
        // Clamp percent between 0 and 100
        if (percent < 0.0f)
            percent = 0.0f;
        if (percent > 100.0f)
            percent = 100.0f;

        int width = 2 * height;      // Width of the battery
        int halfHeight = height / 2; // Half of the height
        int tabWidth = int(0.1 * width);

        // Draw a shape of a battery
        renderer->fillRect(x, y + (halfHeight / 2), tabWidth, halfHeight, ZYN_WHITE);
        renderer->fillRect((x + tabWidth), y, width, height, ZYN_WHITE);
        renderer->fillRect((x + 2 * tabWidth), y + tabWidth, width - 2 * tabWidth, height - 2 * tabWidth, ZYN_BLACK);

        // Calculate width of battery bar
        int barWidth = (int)((width - 4 * tabWidth) * (percent / 100.0f));
        // Draw the battery bar from right to left (opposite direction)
        renderer->fillRect(x + width - barWidth - tabWidth, y + 2 * tabWidth, barWidth, halfHeight / 2, color);
    }

    void drawCursor(int x, int y) {
        // Draw a small circle at the cursor position
        renderer->drawLine(x - 5, y, x + 5, y, ZYN_BLACK);
        renderer->drawLine(x, y - 5, x, y + 5, ZYN_BLACK);
        renderer->drawLine(x - 5, y - 5, x + 5, y + 5, ZYN_BLACK);
        renderer->drawLine(x - 5, y + 5, x + 5, y - 5, ZYN_BLACK);
    }

    void lockScreen()
    {
        // Draw the wallpaper
        renderer->drawTextureToBox(&wallpaper, 0, lockScreenSlidePosition, screenWidth, screenHeight);
        drawBatteryStatus((screenWidth - 40), lockScreenSlidePosition + (screenHeight - 20), 16, 100.0f, 1);

        // Display the Current Time at bottom right corner above the white bar
        char timeString[10];
        snprintf(timeString, sizeof(timeString), "%02d:%02d", 12, 34);
        renderer->printText(screenWidth - 40, lockScreenSlidePosition + (20), timeString, 30, ZYN_BLACK, ZYN_WHITE);

        // Draw a small white bar at the bottom of the screen with a up arrow in center
        renderer->fillRect(0, lockScreenSlidePosition, screenWidth, 20, ZYN_WHITE);
        renderer->fillTriangle((screenWidth / 2) - 10, lockScreenSlidePosition + 15, (screenWidth / 2) + 10, lockScreenSlidePosition + 15, (screenWidth / 2), lockScreenSlidePosition + 5, ZYN_BLACK);
    }

    void update()
    {
        lockScreen();
        drawCursor(cursorPos.x, cursorPos.y);
    }
};

#endif // ZYNGUI_H