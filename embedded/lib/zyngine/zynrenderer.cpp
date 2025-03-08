#include "ZynRenderer.h"

ZynRenderer::ZynRenderer(TFT_eSPI *lcd_display, int screenWidth, int screenHeight)
{
    this->screenWidth = screenWidth;
    this->screenHeight = screenHeight;
    this->lcd_display = lcd_display;

    this->lcd_display->init();
    this->lcd_display->setRotation(0);
}

void ZynRenderer::clear(uint32_t color)
{
    lcd_display->fillScreen(color);
}

void ZynRenderer::printText(int x, int y, const char *text, uint16_t backgroundColor, uint16_t textColor)
{
    lcd_display->setCursor(x, y, 2);
    lcd_display->setTextColor(backgroundColor, textColor);
    lcd_display->setTextSize(1);
    lcd_display->print(text);
}

void ZynRenderer::drawPixel(int x, int y, uint32_t color)
{
    lcd_display->drawPixel(x, y, color);
}

void ZynRenderer::drawLine(int x1, int y1, int x2, int y2, uint32_t color)
{
    lcd_display->drawLine(x1, y2, x2, y2, color);
}

void ZynRenderer::drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color)
{
    lcd_display->drawTriangle(x1, y1, x2, y2, x3, y3, color);
}

void ZynRenderer::fillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint32_t color)
{
    lcd_display->fillTriangle(x1, y1, x2, y2, x3, y3, color);
}

void ZynRenderer::drawTexture(int x, int y, ZynTexture *texture)
{
    Serial.println("TODO: Implement DrawTexture Function");
}