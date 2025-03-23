#include "ZynRenderer.h"

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