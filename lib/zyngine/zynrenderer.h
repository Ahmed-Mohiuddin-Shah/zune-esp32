#ifndef ZYNRENDERER_H
#define ZYNRENDERER_H

#include <config_user.h>
#include <zyncolor.h>
#include <zynmath.h>
#include <zynlight.h>
#include <zynutils.h>
#include <zynvertex.h>
#include <zyncamera.h>

#ifdef ZYNGINE_ESP32S3
#include <LovyanGFX.hpp>
#endif

#ifdef ZYNGINE_NATIVE_RAYLIB
#include <raylib/raylib.h>
#include <cstdint>
#endif

// Render flags
// int RENDER_CCW = 1 << 0;
// int RENDER_BACKGROUND = 1 << 1;
// int RENDER_FACE_NORMAL = 1 << 2;
// int CALC_LIGHT = 1 << 3;
// int RENDER_VERTEX_NORMAL = 1 << 4;
// int RENDER_TANGENT_SPACE = 1 << 5;
// int FLIP_NORMALMAP_Y = 1 << 6;
// int DISABLE_NORMAL_MAPPING = 1 << 7;

// #define DEBUG_NORMAL_LENGTH = 0.1;

class ZynRenderer
{
private:
    int screenHeight;
    int screenWidth;
    float FOV;
    int zBufferLength;

    ZynCamera camera;

    float zClipNear = 0.2f;
    float *zBuffer;

    float intensity = 1.1f;
    ZVec3 dir = ZVec3(1.0f, 1.0f, 0.7f).normalized().mul(ZVec3(-1.0f, 0.0f, 0.0f));
    ZDirectionalLight sun = ZDirectionalLight(intensity, dir);

    float ambient = 0.25f;
    int specularIntensity = 1000;

    ZMat4 transform;

    // ZynTexture diffuseMap;
    // ZynTexture normalMap;

    // Tangential Matrix
    ZMat4 tbn = ZMat4();
    int renderFlag = 0;

#ifdef ZYNGINE_ESP32S3
    lgfx::LGFX_Device *lcd_display;
    LGFX_Sprite *previousFrame;
    LGFX_Sprite *currentFrame;
#endif
#ifdef ZYNGINE_NATIVE_RAYLIB
    Texture2D bitmap;
#endif

public:
#ifdef ZYNGINE_NATIVE_RAYLIB
    ZynRenderer(int screenWidth, int screenHeight, float FOV = 60.0f);
#endif
#ifdef ZYNGINE_ESP32S3
    ZynRenderer(int screenWidth, int screenHeight, lgfx::LGFX_Device *lcd_display);
    void diffDraw();
#endif
    void clear(uint16_t clearColor);
    void printText(int x, int y, const char *text, uint16_t backgroundColor, uint16_t textColor);
    ZVertex viewTransform(ZVertex v);
    ZVertex modelTransform(ZVertex v);
    void renderPixel(ZVec3 p, ZVec3 c);
    void drawPixel(int x, int y, uint16_t color);
    void drawPoint(ZVertex v);
    void drawLine(int x1, int y1, int x2, int y2, uint16_t color);
    void drawRect(int x, int y, int width, int height, uint16_t color);
    void fillRect(int x, int y, int width, int height, uint16_t color);
    void drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint16_t color);
    void fillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint16_t color);
};

#endif