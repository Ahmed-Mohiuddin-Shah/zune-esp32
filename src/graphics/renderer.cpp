#include "zyngine/graphics/renderer.hpp"

#include <algorithm>
#include <cfloat>
#include <cmath>
#include <cstring>

namespace zyngine {
namespace {

// Minimal 5x7 font for ASCII 32..126 (subset: digits, letters, punctuation).
// Each glyph is 5 columns packed into the low 7 bits of 5 bytes (top bit unused).
const uint8_t kFont5x7[][5] = {
    {0x00, 0x00, 0x00, 0x00, 0x00},  // space
    {0x00, 0x00, 0x5F, 0x00, 0x00},  // !
    {0x00, 0x07, 0x00, 0x07, 0x00},  // "
    {0x14, 0x7F, 0x14, 0x7F, 0x14},  // #
    {0x24, 0x2A, 0x7F, 0x2A, 0x12},  // $
    {0x23, 0x13, 0x08, 0x64, 0x62},  // %
    {0x36, 0x49, 0x55, 0x22, 0x50},  // &
    {0x00, 0x05, 0x03, 0x00, 0x00},  // '
    {0x00, 0x1C, 0x22, 0x41, 0x00},  // (
    {0x00, 0x41, 0x22, 0x1C, 0x00},  // )
    {0x08, 0x2A, 0x1C, 0x2A, 0x08},  // *
    {0x08, 0x08, 0x3E, 0x08, 0x08},  // +
    {0x00, 0x50, 0x30, 0x00, 0x00},  // ,
    {0x08, 0x08, 0x08, 0x08, 0x08},  // -
    {0x00, 0x60, 0x60, 0x00, 0x00},  // .
    {0x20, 0x10, 0x08, 0x04, 0x02},  // /
    {0x3E, 0x51, 0x49, 0x45, 0x3E},  // 0
    {0x00, 0x42, 0x7F, 0x40, 0x00},  // 1
    {0x42, 0x61, 0x51, 0x49, 0x46},  // 2
    {0x21, 0x41, 0x45, 0x4B, 0x31},  // 3
    {0x18, 0x14, 0x12, 0x7F, 0x10},  // 4
    {0x27, 0x45, 0x45, 0x45, 0x39},  // 5
    {0x3C, 0x4A, 0x49, 0x49, 0x30},  // 6
    {0x01, 0x71, 0x09, 0x05, 0x03},  // 7
    {0x36, 0x49, 0x49, 0x49, 0x36},  // 8
    {0x06, 0x49, 0x49, 0x29, 0x1E},  // 9
    {0x00, 0x36, 0x36, 0x00, 0x00},  // :
    {0x00, 0x56, 0x36, 0x00, 0x00},  // ;
    {0x00, 0x08, 0x14, 0x22, 0x41},  // <
    {0x14, 0x14, 0x14, 0x14, 0x14},  // =
    {0x41, 0x22, 0x14, 0x08, 0x00},  // >
    {0x02, 0x01, 0x51, 0x09, 0x06},  // ?
    {0x32, 0x49, 0x79, 0x41, 0x3E},  // @
    {0x7E, 0x11, 0x11, 0x11, 0x7E},  // A
    {0x7F, 0x49, 0x49, 0x49, 0x36},  // B
    {0x3E, 0x41, 0x41, 0x41, 0x22},  // C
    {0x7F, 0x41, 0x41, 0x22, 0x1C},  // D
    {0x7F, 0x49, 0x49, 0x49, 0x41},  // E
    {0x7F, 0x09, 0x09, 0x01, 0x01},  // F
    {0x3E, 0x41, 0x41, 0x51, 0x32},  // G
    {0x7F, 0x08, 0x08, 0x08, 0x7F},  // H
    {0x00, 0x41, 0x7F, 0x41, 0x00},  // I
    {0x20, 0x40, 0x41, 0x3F, 0x01},  // J
    {0x7F, 0x08, 0x14, 0x22, 0x41},  // K
    {0x7F, 0x40, 0x40, 0x40, 0x40},  // L
    {0x7F, 0x02, 0x04, 0x02, 0x7F},  // M
    {0x7F, 0x04, 0x08, 0x10, 0x7F},  // N
    {0x3E, 0x41, 0x41, 0x41, 0x3E},  // O
    {0x7F, 0x09, 0x09, 0x09, 0x06},  // P
    {0x3E, 0x41, 0x51, 0x21, 0x5E},  // Q
    {0x7F, 0x09, 0x19, 0x29, 0x46},  // R
    {0x46, 0x49, 0x49, 0x49, 0x31},  // S
    {0x01, 0x01, 0x7F, 0x01, 0x01},  // T
    {0x3F, 0x40, 0x40, 0x40, 0x3F},  // U
    {0x1F, 0x20, 0x40, 0x20, 0x1F},  // V
    {0x7F, 0x20, 0x18, 0x20, 0x7F},  // W
    {0x63, 0x14, 0x08, 0x14, 0x63},  // X
    {0x03, 0x04, 0x78, 0x04, 0x03},  // Y
    {0x61, 0x51, 0x49, 0x45, 0x43},  // Z
};

int fontIndex(char c) {
    if (c < 32 || c > 90) {
        if (c >= 'a' && c <= 'z') {
            return (c - 'a') + ('A' - 32);
        }
        return 0;
    }
    return c - 32;
}

}  // namespace

Renderer::Renderer(int screenWidth, int screenHeight, hal::Display& display)
    : screenWidth_(screenWidth),
      screenHeight_(screenHeight),
      display_(display) {
    zDepthBufferLength_ = screenWidth_ * screenHeight_;
    zDepthBuffer_.assign(static_cast<size_t>(zDepthBufferLength_), -FLT_MAX);
    pixels_.assign(static_cast<size_t>(zDepthBufferLength_), 0);
}

Renderer::~Renderer() = default;

void Renderer::beginFrame() {
    // no-op for software path; reserved for future double-buffering hooks
}

void Renderer::endFrame() {
    display_.present(pixels_.data(), screenWidth_, screenHeight_);
}

void Renderer::setZBuffer(int x, int y, float f) {
    if (x < 0 || y < 0 || x >= screenWidth_ || y >= screenHeight_) {
        return;
    }
    zDepthBuffer_[static_cast<size_t>(x + y * screenWidth_)] = f;
}

float Renderer::getZBuffer(int x, int y) const {
    if (x < 0 || y < 0 || x >= screenWidth_ || y >= screenHeight_) {
        return FLT_MAX;
    }
    return zDepthBuffer_[static_cast<size_t>(x + y * screenWidth_)];
}

bool Renderer::inBounds(int x, int y) const {
    return x >= 0 && y >= 0 && x < screenWidth_ && y < screenHeight_;
}

bool Renderer::inClip(int x, int y) const {
    if (!clipEnabled_) {
        return true;
    }
    return x >= clipX_ && y >= clipY_ && x < clipX_ + clipW_ && y < clipY_ + clipH_;
}

void Renderer::setClipRect(int x, int y, int w, int h) {
    clipEnabled_ = true;
    clipX_ = x;
    clipY_ = y;
    clipW_ = w;
    clipH_ = h;
}

void Renderer::clearClipRect() {
    clipEnabled_ = false;
}

ZVec3 Renderer::barycentricCoordinate(ZVec3* pts, ZVec3 P) {
    ZVec3 u = ZVec3(pts[2].x - pts[0].x, pts[1].x - pts[0].x, pts[0].x - P.x)
                  .cross(ZVec3(pts[2].y - pts[0].y, pts[1].y - pts[0].y, pts[0].y - P.y));
    if (std::abs(u.z) < 1) {
        return ZVec3(-1, 1, 1);
    }
    return ZVec3(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
}

ZVec3 Renderer::world2screen(ZVec3 v) {
    return ZVec3(int((v.x + 1.) * screenWidth_ / 2. + .5),
                 int((v.y + 1.) * screenHeight_ / 2. + .5), v.z);
}

void Renderer::renderTriangle(ZVec3* pts, uint16_t color) {
    ZVec2 bboxmin(FLT_MAX, FLT_MAX);
    ZVec2 bboxmax(-FLT_MAX, -FLT_MAX);
    ZVec2 clamp(static_cast<float>(screenWidth_ - 1), static_cast<float>(screenHeight_ - 1));
    for (int i = 0; i < 3; i++) {
        bboxmin.x = std::max(0.0f, std::min(bboxmin.x, pts[i].x));
        bboxmax.x = std::min(clamp.x, std::max(bboxmax.x, pts[i].x));
        bboxmin.y = std::max(0.0f, std::min(bboxmin.y, pts[i].y));
        bboxmax.y = std::min(clamp.y, std::max(bboxmax.y, pts[i].y));
    }
    ZVec3 P;
    for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++) {
        for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++) {
            ZVec3 bc_screen = barycentricCoordinate(pts, P);
            if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) {
                continue;
            }
            P.z = pts[0].z * bc_screen.x + pts[1].z * bc_screen.y + pts[2].z * bc_screen.z;
            if (getZBuffer(static_cast<int>(P.x), static_cast<int>(P.y)) < P.z) {
                setZBuffer(static_cast<int>(P.x), static_cast<int>(P.y), P.z);
                drawPixel(static_cast<int>(P.x), static_cast<int>(P.y), color);
            }
        }
    }
}

void Renderer::renderTexturedTriangle(ZVec3i* pts, ZVec2i* tpts, float* intensities, Texture* texture) {
    if (pts[0].y == pts[1].y && pts[0].y == pts[2].y) {
        return;
    }
    if (pts[0].y > pts[1].y) {
        std::swap(pts[0], pts[1]);
        std::swap(tpts[0], tpts[1]);
        std::swap(intensities[0], intensities[1]);
    }
    if (pts[0].y > pts[2].y) {
        std::swap(pts[0], pts[2]);
        std::swap(tpts[0], tpts[2]);
        std::swap(intensities[0], intensities[2]);
    }
    if (pts[1].y > pts[2].y) {
        std::swap(pts[1], pts[2]);
        std::swap(tpts[1], tpts[2]);
        std::swap(intensities[1], intensities[2]);
    }

    int total_height = pts[2].y - pts[0].y;
    if (total_height > screenHeight_) {
        return;
    }
    for (int i = 0; i < total_height; i++) {
        bool second_half = i > pts[1].y - pts[0].y || pts[1].y == pts[0].y;
        int segment_height = second_half ? pts[2].y - pts[1].y : pts[1].y - pts[0].y;
        float alpha = static_cast<float>(i) / static_cast<float>(total_height);
        float beta =
            static_cast<float>(i - (second_half ? pts[1].y - pts[0].y : 0)) /
            static_cast<float>(segment_height);

        ZVec3i A = pts[0].add((ZVec3(pts[2].sub(pts[0])).mul(alpha)).toZVec3i());
        ZVec3i B = second_half ? pts[1].add((ZVec3(pts[2].sub(pts[1])).mul(beta)).toZVec3i())
                               : pts[0].add((ZVec3(pts[1].sub(pts[0])).mul(beta)).toZVec3i());
        ZVec2i uvA = tpts[0].add((tpts[2].sub(tpts[0])).mul(alpha));
        ZVec2i uvB = second_half ? tpts[1].add((tpts[2].sub(tpts[1])).mul(beta))
                                 : tpts[0].add((tpts[1].sub(tpts[0])).mul(beta));
        float ityA = intensities[0] + (intensities[2] - intensities[0]) * alpha;
        float ityB = second_half ? intensities[1] + (intensities[2] - intensities[1]) * beta
                                 : intensities[0] + (intensities[1] - intensities[0]) * beta;

        if (A.x > B.x) {
            std::swap(A, B);
            std::swap(uvA, uvB);
            std::swap(ityA, ityB);
        }

        for (int j = A.x; j <= B.x; j++) {
            float phi = B.x == A.x ? 1.f : static_cast<float>(j - A.x) / static_cast<float>(B.x - A.x);
            ZVec3i P = (ZVec3(A).add(ZVec3(B.sub(A)).mul(phi))).toZVec3i();
            ZVec2i uvP = uvA.add((uvB.sub(uvA)).mul(phi));
            float ityP = ityA + (ityB - ityA) * phi;
            ityP = (ityP < 0.1f) ? 0.1f : (ityP > 1.0f ? 1.0f : ityP);
            if (!inBounds(P.x, P.y)) {
                continue;
            }
            if (getZBuffer(P.x, P.y) < P.z) {
                setZBuffer(P.x, P.y, P.z);
                drawPixel(P.x, P.y, intensityRgb565(ityP, texture->getPixel(uvP.x, uvP.y)));
            }
        }
    }
}

void Renderer::renderSphere(ZVec3 pos, uint16_t color) {
    int x = static_cast<int>(pos.x);
    int y = static_cast<int>(pos.y);
    if (getZBuffer(x, y) < pos.z) {
        setZBuffer(x, y, pos.z);
        drawPixel(x, y, color);
    }
}

void Renderer::clear(uint16_t color) {
    std::fill(zDepthBuffer_.begin(), zDepthBuffer_.end(), -FLT_MAX);
    std::fill(pixels_.begin(), pixels_.end(), color);
}

void Renderer::printText(int x, int y, const char* text, uint16_t /*backgroundColor*/, uint16_t textColor) {
    drawText(x, y, text, textColor, 2);
}

void Renderer::printText(int x, int y, const char* text, int size, uint16_t /*backgroundColor*/,
                         uint16_t textColor) {
    drawText(x, y, text, textColor, std::max(1, size));
}

void Renderer::drawPixel(int x, int y, uint16_t color) {
    if (!inBounds(x, y) || !inClip(x, y)) {
        return;
    }
    pixels_[static_cast<size_t>(x + y * screenWidth_)] = color;
}

void Renderer::drawLine(int x1, int y1, int x2, int y2, uint16_t color) {
    int dx = std::abs(x2 - x1);
    int sx = x1 < x2 ? 1 : -1;
    int dy = -std::abs(y2 - y1);
    int sy = y1 < y2 ? 1 : -1;
    int err = dx + dy;
    while (true) {
        drawPixel(x1, y1, color);
        if (x1 == x2 && y1 == y2) {
            break;
        }
        int e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x1 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y1 += sy;
        }
    }
}

void Renderer::drawRect(int x, int y, int width, int height, uint16_t color) {
    drawLine(x, y, x + width - 1, y, color);
    drawLine(x, y + height - 1, x + width - 1, y + height - 1, color);
    drawLine(x, y, x, y + height - 1, color);
    drawLine(x + width - 1, y, x + width - 1, y + height - 1, color);
}

void Renderer::fillRect(int x, int y, int width, int height, uint16_t color) {
    for (int yy = y; yy < y + height; ++yy) {
        for (int xx = x; xx < x + width; ++xx) {
            drawPixel(xx, yy, color);
        }
    }
}

void Renderer::drawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint16_t color) {
    drawLine(x1, y1, x2, y2, color);
    drawLine(x2, y2, x3, y3, color);
    drawLine(x3, y3, x1, y1, color);
}

void Renderer::fillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint16_t color) {
    ZVec3 pts[3] = {ZVec3(static_cast<float>(x1), static_cast<float>(y1), 0),
                    ZVec3(static_cast<float>(x2), static_cast<float>(y2), 0),
                    ZVec3(static_cast<float>(x3), static_cast<float>(y3), 0)};
    // Temporarily bypass z so 2D fills always land
    float saved = -FLT_MAX;
    (void)saved;
    ZVec2 bboxmin(FLT_MAX, FLT_MAX);
    ZVec2 bboxmax(-FLT_MAX, -FLT_MAX);
    for (int i = 0; i < 3; i++) {
        bboxmin.x = std::min(bboxmin.x, pts[i].x);
        bboxmax.x = std::max(bboxmax.x, pts[i].x);
        bboxmin.y = std::min(bboxmin.y, pts[i].y);
        bboxmax.y = std::max(bboxmax.y, pts[i].y);
    }
    for (int y = static_cast<int>(bboxmin.y); y <= static_cast<int>(bboxmax.y); ++y) {
        for (int x = static_cast<int>(bboxmin.x); x <= static_cast<int>(bboxmax.x); ++x) {
            ZVec3 bc = barycentricCoordinate(pts, ZVec3(static_cast<float>(x), static_cast<float>(y), 0));
            if (bc.x >= 0 && bc.y >= 0 && bc.z >= 0) {
                drawPixel(x, y, color);
            }
        }
    }
}

void Renderer::drawTexture(Texture* texture, int x, int y) {
    for (int yp = 0; yp < texture->resolution; ++yp) {
        for (int xp = 0; xp < texture->resolution; ++xp) {
            drawPixel(xp + x, yp + y, texture->getPixel(static_cast<uint16_t>(xp), static_cast<uint16_t>(yp)));
        }
    }
}

void Renderer::drawTexture(Texture* texture, int x, int y, float scaleX, float scaleY) {
    int scaledWidth = static_cast<int>(texture->resolution * scaleX);
    int scaledHeight = static_cast<int>(texture->resolution * scaleY);
    for (int yp = 0; yp < scaledHeight; ++yp) {
        for (int xp = 0; xp < scaledWidth; ++xp) {
            int src_x = static_cast<int>(xp / scaleX);
            int src_y = static_cast<int>(yp / scaleY);
            if (src_x >= texture->resolution) src_x = texture->resolution - 1;
            if (src_y >= texture->resolution) src_y = texture->resolution - 1;
            drawPixel(xp + x, yp + y,
                      texture->getPixel(static_cast<uint16_t>(src_x), static_cast<uint16_t>(src_y)));
        }
    }
}

void Renderer::drawTextureToBox(Texture* texture, int x, int y, int fitWidth, int fitHeight) {
    float scaleX = static_cast<float>(fitWidth) / texture->resolution;
    float scaleY = static_cast<float>(fitHeight) / texture->resolution;
    drawTexture(texture, x, y, scaleX, scaleY);
}

void Renderer::fillCircle(int x, int y, int r, uint16_t color) {
    for (int yy = -r; yy <= r; ++yy) {
        for (int xx = -r; xx <= r; ++xx) {
            if (xx * xx + yy * yy <= r * r) {
                drawPixel(x + xx, y + yy, color);
            }
        }
    }
}

void Renderer::drawText(int x, int y, const char* text, uint16_t color, int scale) {
    if (!text || scale < 1) {
        return;
    }
    int cursor = x;
    for (const char* p = text; *p; ++p) {
        if (*p == '\n') {
            cursor = x;
            y += 8 * scale;
            continue;
        }
        int idx = fontIndex(*p);
        if (idx < 0 || idx >= static_cast<int>(sizeof(kFont5x7) / sizeof(kFont5x7[0]))) {
            idx = 0;
        }
        const uint8_t* glyph = kFont5x7[idx];
        for (int col = 0; col < 5; ++col) {
            uint8_t bits = glyph[col];
            for (int row = 0; row < 7; ++row) {
                if (bits & (1 << row)) {
                    for (int sy = 0; sy < scale; ++sy) {
                        for (int sx = 0; sx < scale; ++sx) {
                            drawPixel(cursor + col * scale + sx, y + row * scale + sy, color);
                        }
                    }
                }
            }
        }
        cursor += 6 * scale;
    }
}

}  // namespace zyngine
