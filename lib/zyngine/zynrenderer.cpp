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
#ifdef ZYNGINE_NATIVE_RAYLIB
ZynRenderer::ZynRenderer(int screenWidth, int screenHeight, float FOV)
{
    InitWindow(screenWidth, screenHeight, "Zyngine");
    SetTargetFPS(60);
    this->screenWidth = screenWidth;
    this->screenHeight = screenHeight;
    this->FOV = FOV;
    zBufferLength = screenWidth * screenHeight;
    zBuffer = new float[zBufferLength];
}
#endif

void ZynRenderer::clear(uint16_t color)
{

    for (int i = 0; i < zBufferLength; i++)
    {
        zBuffer[i] = 3.4028235e+38f; // Maximum value for a 32-bit float
    }

#ifdef ZYNGINE_ESP32S3
    // TODO
    currentFrame->clear();
#endif
#ifdef ZYNGINE_NATIVE_RAYLIB
    ClearBackground(getRaylibColorFromRGB565(color));
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

#ifdef ZYNGINE_NATIVE_RAYLIB
    DrawText(text, x, y, 20, WHITE);
#endif
}

void ZynRenderer::drawPixel(int x, int y, uint16_t color)
{
#ifdef ZYNGINE_ESP32S3
    currentFrame->drawPixel(x, y, color);
#endif

#ifdef ZYNGINE_NATIVE_RAYLIB
    DrawPixel(x, y, getRaylibColorFromRGB565(color));
#endif
}

ZVertex ZynRenderer::modelTransform(ZVertex v)
{
    ZVec3 newPos = transform.mulVector(v.position, 1.0);
    ZVec3 newNor = transform.mulVector(v.normal, 0.0).normalized();
    ZVec3 newTan = transform.mulVector(v.tangent, 0.0).normalized();
    ZVec3 newBiTan = transform.mulVector(v.biTangent, 0.0).normalized();

    return ZVertex(newPos, v.color, v.texCoord, newNor, newTan, newBiTan);
}

ZVertex ZynRenderer::viewTransform(ZVertex v)
{
    ZVec3 newPos = camera.cameraTransform.mulVector(ZVec3(v.position.x, v.position.y, v.position.z));
    newPos.z *= -1.0;

    ZVec3 newNor = camera.cameraTransform.mulVector(v.normal, 0.0).normalized();
    ZVec3 newTan = camera.cameraTransform.mulVector(v.tangent, 0.0).normalized();
    ZVec3 newBiTan = camera.cameraTransform.mulVector(v.biTangent, 0.0).normalized();

    return ZVertex(newPos, v.color, v.texCoord, newNor, newTan, newBiTan);
}

void ZynRenderer::drawPoint(ZVertex v)
{
    v = viewTransform(v);
    if (v.position.z < zClipNear)
    {
        return;
    }

    int sx = (v.position.x / v.position.z * FOV + screenWidth / 2.0);
    int sy = (v.position.y / v.position.z * FOV + screenHeight / 2.0);

    renderPixel(ZVec3(sx, sy, v.position.z), v.color);
}

void ZynRenderer::drawLine(ZVertex v0, ZVertex v1){
    v0 = viewTransform(v0);
    v1 = viewTransform(v1);

    // z-Near clipping
    if (v0.position.z < zClipNear && v1.position.z < zClipNear)
    {
        return;
    }

    if (v0.position.z < zClipNear)
    {
        float per = (zClipNear - v0.position.z) / (v1.position.z - v0.position.z);

        v0.position = v0.position.add(v1.position.sub(v0.position).mul(per));
        v0.color = lerpVector2(v0.color, v1.color, per);
    }

    if (v1.position.z < zClipNear)
    {
        float per = (zClipNear - v1.position.z) / (v0.position.z - v1.position.z);

        v1.position = v1.position.add(v0.position.sub(v1.position).mul(per));
        v1.color = lerpVector2(v1.color, v0.color, per);
    }

    // Transform a vertices in camera space to viewport space at one time (Avoid matrix multiplication)
    // Projection transform + viewport transform

    ZVec2 p0 = ZVec2(
        v0.position.x / v0.position.z * FOV + screenWidth / 2.0 - 0.5,
        v0.position.y / v0.position.z * FOV + screenHeight / 2.0 - 0.5);
    ZVec2 p1 = ZVec2(
        v1.position.x / v1.position.z * FOV + screenWidth / 2.0 - 0.5,
        v1.position.y / v1.position.z * FOV + screenHeight / 2.0 - 0.5);

    // Render left to right
    if (p1.x < p0.x)
    {
        ZVec2 tmpVec2 = p0;
        p0 = p1;
        p1 = tmpVec2;

        ZVertex tmpZVertex = v0;
        v0 = v1;
        v1 = tmpZVertex;
    }

    float x0 = ceil(p0.x);
    float y0 = ceil(p0.y);
    float x1 = ceil(p1.x);
    float y1 = ceil(p1.y);

    float dx = p1.x - p0.x;
    float dy = p1.y - p0.y;

    float m = abs(dy / dx);

    if (m <= 1)
    {
        for (int x = x0; x < x1; ++x)
        {
            float per = (x - p0.x) / (p1.x - p0.x);

            float y = p0.y + (p1.y - p0.y) * per;
            float z = 1 / ((1 - per) / v0.position.z + per / v1.position.z);

            ZVec3 c = lerp2AttributeVec3(v0.color, v1.color, (1 - per), per, v0.position.z, v1.position.z, z);

            renderPixel(ZVec3(ceil(x), ceil(y), z), c);
        }
    }
    else
    {
        if (p1.y < p0.y)
        {
            ZVec2 tmpVec2 = p0;
            p0 = p1;
            p1 = tmpVec2;

            ZVertex tmpZVertex = v0;
            v0 = v1;
            v1 = tmpZVertex;
        }

        x0 = ceil(p0.x);
        y0 = ceil(p0.y);
        x1 = ceil(p1.x);
        y1 = ceil(p1.y);

        if (x0 < 0)
            x0 = 0;
        if (x1 > screenWidth)
            x1 = screenWidth;
        if (y0 < 0)
            y0 = 0;
        if (y1 > screenHeight)
            y1 = screenHeight;

        for (int y = y0; y < y1; ++y)
        {
            float per = (y - p0.y) / (p1.y - p0.y);

            float x = p0.x + (p1.x - p0.x) * per;
            float z = 1 / ((1 - per) / v0.position.z + per / v1.position.z);

            ZVec3 c = lerp2AttributeVec3(v0.color, v1.color, (1 - per), per, v0.position.z, v1.position.z, z);
            renderPixel(ZVec3(ceil(x), ceil(y), z), c);
        }
    }
}

void ZynRenderer::renderPixel(ZVec3 p, ZVec3 c)
{
    int index = p.x + (screenHeight - 1 - p.y) * screenWidth;
    if (p.z >= zBuffer[index])
    {
        return;
    }

    if (p.x < 0 || p.x >= screenWidth || p.y < 0 || p.y >= screenHeight)
    {
        return;
    }

    // TODO Double check this
    // pixels[p.x + (screenscreenHeight - 1 - p.y) * screenWidth] = c;
    drawPixel(p.x, p.y, getRGB565FromZVec3(c));
    zBuffer[index] = p.z;
}

void ZynRenderer::drawLine(int x1, int y1, int x2, int y2, uint16_t color)
{
#ifdef ZYNGINE_ESP32S3
    currentFrame->drawLine(x1, y2, x2, y2, color);
#endif

#ifdef ZYNGINE_NATIVE_RAYLIB

    DrawLine(x1, y1, x2, y2, getRaylibColorFromRGB565(color));
#endif
}

void ZynRenderer::drawRect(int x, int y, int width, int height, uint16_t color)
{
#ifdef ZYNGINE_ESP32S3
    currentFrame->drawRect(x, y, width, height, color);
#endif

#ifdef ZYNGINE_NATIVE_RAYLIB

    DrawRectangleLines(x, y, width, height, getRaylibColorFromRGB565(color));
#endif
}

void ZynRenderer::fillRect(int x, int y, int width, int height, uint16_t color)
{
#ifdef ZYNGINE_ESP32S3
    currentFrame->fillRect(x, y, width, height, color);
#endif

#ifdef ZYNGINE_NATIVE_RAYLIB

    DrawRectangle(x, y, width, height, getRaylibColorFromRGB565(color));
#endif
}

void ZynRenderer::drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint16_t color)
{
#ifdef ZYNGINE_ESP32S3
    currentFrame->drawTriangle(x1, y1, x2, y2, x3, y3, color);
#endif

#ifdef ZYNGINE_NATIVE_RAYLIB

    DrawTriangleLines({(float)x1, (float)y1}, {(float)x2, (float)y2}, {(float)x3, (float)y3}, getRaylibColorFromRGB565(color));
#endif
}

void ZynRenderer::fillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint16_t color)
{
#ifdef ZYNGINE_ESP32S3
    currentFrame->fillTriangle(x1, y1, x2, y2, x3, y3, color);
#endif

#ifdef ZYNGINE_NATIVE_RAYLIB

    DrawTriangle({(float)x1, (float)y1}, {(float)x2, (float)y2}, {(float)x3, (float)y3}, getRaylibColorFromRGB565(color));
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