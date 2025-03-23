#ifndef ZYNRENDERER_H
#define ZYNRENDERER_H

#include <config_user.h>

#ifdef ZYNGINE_ESP32S3
#include <LovyanGFX.hpp>
#endif

#ifdef ZYNGINE_WINDOWS_NATIVE_RAYLIB_CUSTOM_SOFTWARE_RENDERER
#include <raylib/raylib.h>
#endif

#include <zyntexture.h>

class ZynRenderer
{
private:
    int screenHeight;
    int screenWidth;

#ifdef ZYNGINE_ESP32S3
    lgfx::LGFX_Device *lcd_display;
    LGFX_Sprite *previousFrame;
    LGFX_Sprite *currentFrame;
#endif

public:
#ifdef ZYNGINE_WINDOWS_NATIVE_RAYLIB_CUSTOM_SOFTWARE_RENDERER
    ZynRenderer(int screenWidth, int screenHeight);
#endif
#ifdef ZYNGINE_ESP32S3
    ZynRenderer(int screenWidth, int screenHeight, lgfx::LGFX_Device *lcd_display);
    void diffDraw();
#endif

    void clear();
    void printText(int x, int y, const char *text, uint16_t backgroundColor, uint16_t textColor);
    void drawPixel(int x, int y, uint16_t color);
    void drawLine(int x1, int y1, int x2, int y2, uint16_t color);
    void drawRect(int x, int y, int width, int height, uint16_t color);
    void fillRect(int x, int y, int width, int height, uint16_t color);
    void drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint16_t color);
    void fillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint16_t color);
    void drawTexture(int x, int y, ZynTexture *texture);
};

#endif