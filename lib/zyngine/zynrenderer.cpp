#include "ZynRenderer.h"
#include <algorithm>
#include <iostream>

#ifdef ZYNGINE_ESP32S3
ZynRenderer::ZynRenderer(int screenWidth, int screenHeight, lgfx::LGFX_Device *lcd_display)
{
    this->screenWidth = screenWidth;
    this->screenHeight = screenHeight;
    this->lcd_display = lcd_display;

    lcd_display->begin();
    lcd_display->startWrite();
    lcd_display->setColorDepth(16);
    lcd_display->setRotation(0); // Set rotation to 0 for portrait mode

    currentFrame = new LGFX_Sprite();
    previousFrame = new LGFX_Sprite();

    currentFrame->setTextSize(2);
    currentFrame->setColorDepth(8);
    previousFrame->setTextSize(2);
    previousFrame->setColorDepth(8);

    currentFrame->setPsram(true);
    currentFrame->createSprite(screenWidth, screenHeight);
    // currentFrame->setRotation(0); // No need to set rotation for sprite
    previousFrame->setPsram(true);
    previousFrame->createSprite(screenWidth, screenHeight);
    // previousFrame->setRotation(0); // No need to set rotation for sprite
}
#endif
#ifdef ZYNGINE_WINDOWS_NATIVE_RAYLIB_CUSTOM_SOFTWARE_RENDERER
ZynRenderer::ZynRenderer(int screenWidth, int screenHeight)
{
    InitWindow(screenWidth, screenHeight, "Zyngine");
    SetTargetFPS(60);
    this->screenWidth = screenWidth;
    this->screenHeight = screenHeight;
    pDepthBuffer = new float[screenWidth * screenHeight];
}
#endif

void ZynRenderer::clear()
{
#ifdef ZYNGINE_ESP32S3
    currentFrame->clear();
#endif
#ifdef ZYNGINE_WINDOWS_NATIVE_RAYLIB_CUSTOM_SOFTWARE_RENDERER
    ClearBackground(BLACK);
#endif
}

void ZynRenderer::printText(int x, int y, const char *text, uint16_t backgroundColor, uint16_t textColor)
{
#ifdef ZYNGINE_ESP32S3
    currentFrame->setCursor(x, y, 2);
    currentFrame->setTextColor(backgroundColor, textColor);
    currentFrame->setTextSize(1);
    currentFrame->print(text);
#endif

#ifdef ZYNGINE_WINDOWS_NATIVE_RAYLIB_CUSTOM_SOFTWARE_RENDERER
    DrawText(text, x, y, 20, WHITE);
#endif
}

void ZynRenderer::drawPixel(int x, int y, uint16_t color)
{
#ifdef ZYNGINE_ESP32S3
    currentFrame->drawPixel(x, y, color);
#endif

#ifdef ZYNGINE_WINDOWS_NATIVE_RAYLIB_CUSTOM_SOFTWARE_RENDERER
    // Convert 16-bit color to Raylib Color
    Color raylibColor = {
        (unsigned char)((color >> 11) & 0x1F) * 255 / 31,
        (unsigned char)((color >> 5) & 0x3F) * 255 / 63,
        (unsigned char)(color & 0x1F) * 255 / 31,
        255};
    DrawPixel(x, y, raylibColor);
#endif
}

void ZynRenderer::drawLine(int x1, int y1, int x2, int y2, uint16_t color)
{
#ifdef ZYNGINE_ESP32S3
    currentFrame->drawLine(x1, y2, x2, y2, color);
#endif

#ifdef ZYNGINE_WINDOWS_NATIVE_RAYLIB_CUSTOM_SOFTWARE_RENDERER
    // Convert 16-bit color to Raylib Color
    Color raylibColor = {
        (unsigned char)((color >> 11) & 0x1F) * 255 / 31,
        (unsigned char)((color >> 5) & 0x3F) * 255 / 63,
        (unsigned char)(color & 0x1F) * 255 / 31,
        255};

    DrawLine(x1, y1, x2, y2, raylibColor);
#endif
}

void ZynRenderer::drawRect(int x, int y, int width, int height, uint16_t color)
{
#ifdef ZYNGINE_ESP32S3
    currentFrame->drawRect(x, y, width, height, color);
#endif

#ifdef ZYNGINE_WINDOWS_NATIVE_RAYLIB_CUSTOM_SOFTWARE_RENDERER
    // Convert 16-bit color to Raylib Color
    Color raylibColor = {
        (unsigned char)((color >> 11) & 0x1F) * 255 / 31,
        (unsigned char)((color >> 5) & 0x3F) * 255 / 63,
        (unsigned char)(color & 0x1F) * 255 / 31,
        255};

    DrawRectangleLines(x, y, width, height, raylibColor);
#endif
}

void ZynRenderer::fillRect(int x, int y, int width, int height, uint16_t color)
{
#ifdef ZYNGINE_ESP32S3
    currentFrame->fillRect(x, y, width, height, color);
#endif

#ifdef ZYNGINE_WINDOWS_NATIVE_RAYLIB_CUSTOM_SOFTWARE_RENDERER
    // Convert 16-bit color to Raylib Color
    Color raylibColor = {
        (unsigned char)((color >> 11) & 0x1F) * 255 / 31,
        (unsigned char)((color >> 5) & 0x3F) * 255 / 63,
        (unsigned char)(color & 0x1F) * 255 / 31,
        255};

    DrawRectangle(x, y, width, height, raylibColor);
#endif
}

void ZynRenderer::drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint16_t color)
{
#ifdef ZYNGINE_ESP32S3
    currentFrame->drawTriangle(x1, y1, x2, y2, x3, y3, color);
#endif

#ifdef ZYNGINE_WINDOWS_NATIVE_RAYLIB_CUSTOM_SOFTWARE_RENDERER
    // Convert 16-bit color to Raylib Color
    Color raylibColor = {
        (unsigned char)((color >> 11) & 0x1F) * 255 / 31,
        (unsigned char)((color >> 5) & 0x3F) * 255 / 63,
        (unsigned char)(color & 0x1F) * 255 / 31,
        255};

    DrawTriangleLines({(float)x1, (float)y1}, {(float)x2, (float)y2}, {(float)x3, (float)y3}, raylibColor);
#endif
}

void ZynRenderer::fillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint16_t color)
{
#ifdef ZYNGINE_ESP32S3
    currentFrame->fillTriangle(x1, y1, x2, y2, x3, y3, color);
#endif

#ifdef ZYNGINE_WINDOWS_NATIVE_RAYLIB_CUSTOM_SOFTWARE_RENDERER
    // Convert 16-bit color to Raylib Color
    Color raylibColor = {
        (unsigned char)((color >> 11) & 0x1F) * 255 / 31,
        (unsigned char)((color >> 5) & 0x3F) * 255 / 63,
        (unsigned char)(color & 0x1F) * 255 / 31,
        255};

    DrawTriangle({(float)x1, (float)y1}, {(float)x2, (float)y2}, {(float)x3, (float)y3}, raylibColor);
#endif
}

void ZynRenderer::drawTexture(int x, int y, ZynTexture *texture)
{
#ifdef ZYNGINE_ESP32S3
    Serial.println("TODO: Implement DrawTexture Function");
    // currentFrame->drawBitmap(x, y, texture->getWidth(), texture->getHeight(), texture->getBuffer());
#endif
#ifdef ZYNGINE_WINDOWS_NATIVE_RAYLIB_CUSTOM_SOFTWARE_RENDERER
    for (int i = 0; i < texture->height; i++)
    {
        for (int j = 0; j < texture->width; j++)
        {
            drawPixel(x + j, y + i, texture->GetPixel(j, i));
        }
    }
#endif
}

void ZynRenderer::drawTexturedTriangle(int x1, int y1, float u1, float v1, float w1,
                                       int x2, int y2, float u2, float v2, float w2,
                                       int x3, int y3, float u3, float v3, float w3,
                                       ZynTexture *texture)
{
#ifdef ZYNGINE_WINDOWS_NATIVE_RAYLIB_CUSTOM_SOFTWARE_RENDERER
    // Calculate bounding box of the triangle
    int minX = std::min({x1, x2, x3});
    int minY = std::min({y1, y2, y3});
    int maxX = std::max({x1, x2, x3});
    int maxY = std::max({y1, y2, y3});

    // Loop through each pixel in the bounding box
    for (int y = minY; y <= maxY; y++)
    {
        for (int x = minX; x <= maxX; x++)
        {
            // Calculate barycentric coordinates
            float lambda1 = ((float)(y2 - y3) * (x - x3) + (float)(x3 - x2) * (y - y3)) /
                            ((float)(y2 - y3) * (x1 - x3) + (float)(x3 - x2) * (y1 - y3));
            float lambda2 = ((float)(y3 - y1) * (x - x3) + (float)(x1 - x3) * (y - y3)) /
                            ((float)(y2 - y3) * (x1 - x3) + (float)(x3 - x2) * (y1 - y3));
            float lambda3 = 1.0f - lambda1 - lambda2;

            // Check if the pixel is inside the triangle
            if (lambda1 >= 0 && lambda2 >= 0 && lambda3 >= 0)
            {
                // Interpolate depth
                float z = lambda1 * w1 + lambda2 * w2 + lambda3 * w3;
                int index = y * screenWidth + x;

                // // Depth test
                if (z < pDepthBuffer[index])
                {
                    // Interpolate texture coordinates
                    float u = lambda1 * u1 + lambda2 * u2 + lambda3 * u3;
                    float v = lambda1 * v1 + lambda2 * v2 + lambda3 * v3;

                    // // Get texture color
                    uint16_t color = texture->GetPixel(u * texture->width, v * texture->height);

                    // // Draw pixel
                    drawPixel(x, y, color);

                    // Update depth buffer
                    pDepthBuffer[index] = z;
                }
            }
        }
    }
#endif
}

#ifdef ZYNGINE_ESP32S3
void ZynRenderer::diffDraw()
{
    union
    {
        std::uint32_t *s32;
        std::uint8_t *s;
    };
    union
    {
        std::uint32_t *p32;
        std::uint8_t *p;
    };
    s32 = (std::uint32_t *)currentFrame->getBuffer();
    p32 = (std::uint32_t *)previousFrame->getBuffer();

    auto width = currentFrame->width();
    auto height = currentFrame->height();

    auto w32 = (width + 3) >> 2;
    std::int32_t y = 0;
    do
    {
        std::int32_t x32 = 0;
        do
        {
            while (s32[x32] == p32[x32] && ++x32 < w32)
                ;
            if (x32 == w32)
                break;

            std::int32_t xs = x32 << 2;
            while (s[xs] == p[xs])
                ++xs;

            while (++x32 < w32 && s32[x32] != p32[x32])
                ;

            std::int32_t xe = (x32 << 2) - 1;
            if (xe >= width)
                xe = width - 1;
            while (s[xe] == p[xe])
                --xe;

            lcd_display->pushImage(xs, y, xe - xs + 1, 1, &s[xs]);
        } while (x32 < w32);
        s32 += w32;
        p32 += w32;
    } while (++y < height);
    lcd_display->display();

    LGFX_Sprite *swap;
    swap = currentFrame;
    currentFrame = previousFrame;
    previousFrame = swap;
}
#endif