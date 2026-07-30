#pragma once

#include <cstdint>
#include <vector>

#include "zyngine/graphics/color.hpp"
#include "zyngine/graphics/math.hpp"
#include "zyngine/graphics/texture.hpp"
#include "zyngine/hal/display.hpp"

namespace zyngine {

/// Theme-agnostic software renderer. Draws into an RGB565 framebuffer and
/// flushes via hal::Display. No platform headers.
class Renderer {
public:
    Renderer(int screenWidth, int screenHeight, hal::Display& display);
    ~Renderer();

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    int getScreenWidth() const { return screenWidth_; }
    int getScreenHeight() const { return screenHeight_; }

    /// Null if framebuffer allocation failed (e.g. ESP32 OOM).
    uint16_t* pixels() { return pixels_; }
    const uint16_t* pixels() const { return pixels_; }
    bool ok() const { return pixels_ != nullptr; }

    void beginFrame();
    void endFrame();

    ZVec3 barycentricCoordinate(ZVec3* pts, ZVec3 P);
    ZVec3 world2screen(ZVec3 v);
    void renderTriangle(ZVec3* pts, uint16_t color);
    void renderTexturedTriangle(ZVec3i* pts, ZVec2i* tpts, float* intensities, Texture* texture);
    void renderSphere(ZVec3 pos, uint16_t color);

    void clear(uint16_t clearColor);
    void printText(int x, int y, const char* text, uint16_t backgroundColor, uint16_t textColor);
    void printText(int x, int y, const char* text, int size, uint16_t backgroundColor, uint16_t textColor);

    void drawPixel(int x, int y, uint16_t color);
    void drawLine(int x1, int y1, int x2, int y2, uint16_t color);
    void drawRect(int x, int y, int width, int height, uint16_t color);
    void fillRect(int x, int y, int width, int height, uint16_t color);
    void drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint16_t color);
    void fillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint16_t color);
    void drawTexture(Texture* texture, int x, int y);
    void drawTexture(Texture* texture, int x, int y, float scaleX, float scaleY);
    void drawTextureToBox(Texture* texture, int x, int y, int fitWidth, int fitHeight);
    void fillCircle(int x, int y, int r, uint16_t color);

    /// Simple 5x7 bitmap text (no external font dependency).
    void drawText(int x, int y, const char* text, uint16_t color, int scale = 1);

    void setClipRect(int x, int y, int w, int h);
    void clearClipRect();

private:
    void setZBuffer(int x, int y, float f);
    float getZBuffer(int x, int y) const;
    bool inBounds(int x, int y) const;
    bool inClip(int x, int y) const;

    int screenWidth_ = 0;
    int screenHeight_ = 0;
    int zDepthBufferLength_ = 0;
    std::vector<float> zDepthBuffer_;
    uint16_t* pixels_ = nullptr;
    hal::Display& display_;

    bool clipEnabled_ = false;
    int clipX_ = 0;
    int clipY_ = 0;
    int clipW_ = 0;
    int clipH_ = 0;
};

// Legacy alias
using ZynRenderer = Renderer;

}  // namespace zyngine
