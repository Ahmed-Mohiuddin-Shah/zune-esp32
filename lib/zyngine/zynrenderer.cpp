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
    for (int i = 0; i < screenWidth * screenHeight; i++)
        pDepthBuffer[i] = 0.0f;
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
    if (x < 0 || y < 0)
    {
        return;
    }
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
    if (y2 < y1)
    {
        std::swap(y1, y2);
        std::swap(x1, x2);
        std::swap(u1, u2);
        std::swap(v1, v2);
        std::swap(w1, w2);
    }

    if (y3 < y1)
    {
        std::swap(y1, y3);
        std::swap(x1, x3);
        std::swap(u1, u3);
        std::swap(v1, v3);
        std::swap(w1, w3);
    }

    if (y3 < y2)
    {
        std::swap(y2, y3);
        std::swap(x2, x3);
        std::swap(u2, u3);
        std::swap(v2, v3);
        std::swap(w2, w3);
    }

    int dy1 = y2 - y1;
    int dx1 = x2 - x1;
    float dv1 = v2 - v1;
    float du1 = u2 - u1;
    float dw1 = w2 - w1;

    int dy2 = y3 - y1;
    int dx2 = x3 - x1;
    float dv2 = v3 - v1;
    float du2 = u3 - u1;
    float dw2 = w3 - w1;

    float tex_u, tex_v, tex_w;

    float dax_step = 0, dbx_step = 0,
          du1_step = 0, dv1_step = 0,
          du2_step = 0, dv2_step = 0,
          dw1_step = 0, dw2_step = 0;

    if (dy1)
        dax_step = dx1 / (float)abs(dy1);
    if (dy2)
        dbx_step = dx2 / (float)abs(dy2);

    if (dy1)
        du1_step = du1 / (float)abs(dy1);
    if (dy1)
        dv1_step = dv1 / (float)abs(dy1);
    if (dy1)
        dw1_step = dw1 / (float)abs(dy1);

    if (dy2)
        du2_step = du2 / (float)abs(dy2);
    if (dy2)
        dv2_step = dv2 / (float)abs(dy2);
    if (dy2)
        dw2_step = dw2 / (float)abs(dy2);

    if (dy1)
    {
        for (int i = y1; i <= y2; i++)
        {
            int ax = x1 + (float)(i - y1) * dax_step;
            int bx = x1 + (float)(i - y1) * dbx_step;

            float tex_su = u1 + (float)(i - y1) * du1_step;
            float tex_sv = v1 + (float)(i - y1) * dv1_step;
            float tex_sw = w1 + (float)(i - y1) * dw1_step;

            float tex_eu = u1 + (float)(i - y1) * du2_step;
            float tex_ev = v1 + (float)(i - y1) * dv2_step;
            float tex_ew = w1 + (float)(i - y1) * dw2_step;

            if (ax > bx)
            {
                std::swap(ax, bx);
                std::swap(tex_su, tex_eu);
                std::swap(tex_sv, tex_ev);
                std::swap(tex_sw, tex_ew);
            }

            tex_u = tex_su;
            tex_v = tex_sv;
            tex_w = tex_sw;

            float tstep = 1.0f / ((float)(bx - ax));
            float t = 0.0f;

            for (int j = ax; j < bx; j++)
            {
                tex_u = (1.0f - t) * tex_su + t * tex_eu;
                tex_v = (1.0f - t) * tex_sv + t * tex_ev;
                tex_w = (1.0f - t) * tex_sw + t * tex_ew;
                std::cout << i << ", " << j << ", " << tex_u / tex_w << ", " << tex_v / tex_w << std::endl;
                if (tex_w > pDepthBuffer[i * screenWidth + j])
                {
                drawPixel(j, i, texture->GetPixel(tex_u / tex_w, tex_v / tex_w));
                pDepthBuffer[i * screenWidth + j] = tex_w;
                }
                t += tstep;
            }
        }
    }

    dy1 = y3 - y2;
    dx1 = x3 - x2;
    dv1 = v3 - v2;
    du1 = u3 - u2;
    dw1 = w3 - w2;

    if (dy1)
        dax_step = dx1 / (float)abs(dy1);
    if (dy2)
        dbx_step = dx2 / (float)abs(dy2);

    du1_step = 0, dv1_step = 0;
    if (dy1)
        du1_step = du1 / (float)abs(dy1);
    if (dy1)
        dv1_step = dv1 / (float)abs(dy1);
    if (dy1)
        dw1_step = dw1 / (float)abs(dy1);

    if (dy1)
    {
        for (int i = y2; i <= y3; i++)
        {
            int ax = x2 + (float)(i - y2) * dax_step;
            int bx = x1 + (float)(i - y1) * dbx_step;

            float tex_su = u2 + (float)(i - y2) * du1_step;
            float tex_sv = v2 + (float)(i - y2) * dv1_step;
            float tex_sw = w2 + (float)(i - y2) * dw1_step;

            float tex_eu = u1 + (float)(i - y1) * du2_step;
            float tex_ev = v1 + (float)(i - y1) * dv2_step;
            float tex_ew = w1 + (float)(i - y1) * dw2_step;

            if (ax > bx)
            {
                std::swap(ax, bx);
                std::swap(tex_su, tex_eu);
                std::swap(tex_sv, tex_ev);
                std::swap(tex_sw, tex_ew);
            }

            tex_u = tex_su;
            tex_v = tex_sv;
            tex_w = tex_sw;

            float tstep = 1.0f / ((float)(bx - ax));
            float t = 0.0f;

            for (int j = ax; j < bx; j++)
            {
                tex_u = (1.0f - t) * tex_su + t * tex_eu;
                tex_v = (1.0f - t) * tex_sv + t * tex_ev;
                tex_w = (1.0f - t) * tex_sw + t * tex_ew;

                std::cout << i << ", " << j << ", " << tex_u / tex_w << ", " << tex_v / tex_w << std::endl;
                if (tex_w > pDepthBuffer[i * screenWidth + j])
                {
                drawPixel(j, i, texture->GetPixel(tex_u / tex_w, tex_v / tex_w));
                    pDepthBuffer[i * screenWidth + j] = tex_w;
                }
                t += tstep;
            }
        }
    }
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