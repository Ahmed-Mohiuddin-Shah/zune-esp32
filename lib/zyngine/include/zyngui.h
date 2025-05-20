#ifndef ZYNGUI_H
#define ZYNGUI_H

#include "zynrenderer.h"

class ZynGUI
{
private:
    ZynRenderer *renderer;
    int screenWidth;
    int screenHeight;
    int lockScreenSlidePosition = 0;

public:
    void passRenderer(ZynRenderer *renderer)
    {
        this->renderer = renderer;
        screenWidth = renderer->getScreenWidth();
        screenHeight = renderer->getScreenHeight();
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

    void update() {
        drawBatteryStatus(screenWidth - 40, screenHeight - 20, 16, 100.0f, 1);
    }
};

#endif // ZYNGUI_H