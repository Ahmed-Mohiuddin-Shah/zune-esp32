#ifndef ZYNRENDERER_H
#define ZYNRENDERER_H

#include <config_user.h>
#include <zyncolor.h>
#include <zynmath.h>
#include <zynutils.h>
#include <zyntexture.h>
#include <float.h>

#ifdef ZYNGINE_ESP32S3
#include <LovyanGFX.hpp>
#endif

#ifdef ZYNGINE_NATIVE_RAYLIB
#include <raylib/raylib.h>
#include <cstdint>
#endif

class ZynRenderer
{
private:
    int screenHeight;
    int screenWidth;
    int zDepthBufferLength;
    float *zDepthBuffer;

#ifdef ZYNGINE_ESP32S3
    lgfx::LGFX_Device *lcd_display;
    LGFX_Sprite *previousFrame;
    LGFX_Sprite *currentFrame;
#endif
#ifdef ZYNGINE_NATIVE_RAYLIB
#endif

    inline void setZBuffer(int x, int y, float f)
    {
        if (x * y >= zDepthBufferLength)
        {
            return;
        }
        zDepthBuffer[x + y * screenWidth] = f;
    }
    inline float getZBuffer(int x, int y)
    {
        if (x * y >= zDepthBufferLength)
        {
            return FLT_MAX;
        }

        // int x = idx % width;
        // int y = idx / width;

        return zDepthBuffer[x + y * screenWidth];
    }

public:
#ifdef ZYNGINE_NATIVE_RAYLIB
    RenderTexture2D frame;
    ZynRenderer(int screenWidth, int screenHeight);
#endif
#ifdef ZYNGINE_ESP32S3
    ZynRenderer(int screenWidth, int screenHeight, lgfx::LGFX_Device *lcd_display);
    void diffDraw();
#endif
    ZVec3 barycentricCoordinate(ZVec3 *pts, ZVec3 P);
    ZVec3 world2screen(ZVec3 v);
    void renderTriangle(ZVec3 *pts, uint16_t color);
    void renderTexturedTriangle(ZVec3 *pts, ZVec2 *tpts, float intensity, ZynTexture *texture);
    void clear(uint16_t clearColor);
    void printText(int x, int y, const char *text, uint16_t backgroundColor, uint16_t textColor);
    void renderPixel(ZVec3 p, ZVec3 c);
    void drawPixel(int x, int y, uint16_t color);
    void drawLine(int x1, int y1, int x2, int y2, uint16_t color);
    void drawRect(int x, int y, int width, int height, uint16_t color);
    void fillRect(int x, int y, int width, int height, uint16_t color);
    void drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint16_t color);
    void fillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint16_t color);
    void drawTexture(ZynTexture texture, int x, int y); // TODO chack to see is alpha value possible
};

#endif