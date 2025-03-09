#ifndef ZYNRENDERER_H
#define ZYNRENDERER_H

#include <LovyanGFX.hpp>
#include <zyntexture.h>

class ZynRenderer
{
private:
    int screenHeight;
    int screenWidth;
    lgfx::LGFX_Device *lcd_display;
    LGFX_Sprite *currentFrame;
    LGFX_Sprite *previousFrame;

public:
    ZynRenderer(int screenWidth, int screenHeight, lgfx::LGFX_Device *lcd_display);
    void clear();
    void printText(int x, int y, const char *text, uint16_t backgroundColor, uint16_t textColor);
    void drawPixel(int x, int y, uint16_t color);
    void drawLine(int x1, int y1, int x2, int y2, uint16_t color);
    void drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint16_t color);
    void fillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint16_t color);
    void drawTexture(int x, int y, ZynTexture *texture);
    void diffDraw();
};

#endif