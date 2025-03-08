#ifndef ZYNRENDERER_H
#define ZYNRENDERER_H

#include <cstdint>
#include <zyntexture.h>
#include <SPI.h>
#include "FS.h"
#include <TFT_eSPI.h>

class ZynRenderer
{
private:
    int screenHeight;
    int screenWidth;

public:
    TFT_eSPI *lcd_display;
    ZynRenderer(TFT_eSPI *lcd_display, int screenWidth, int screenHeight);
    void clear(uint32_t color);
    void printText(int x, int y, const char *text, uint16_t backgroundColor, uint16_t textColor);
    void drawPixel(int x, int y, uint32_t color);
    void drawLine(int x1, int y1, int x2, int y2, uint32_t color);
    void drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color);
    void fillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color);
    void drawTexture(int x, int y, ZynTexture *texture);
};

#endif