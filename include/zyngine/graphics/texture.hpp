#pragma once

#include <cstdint>

namespace zyngine {

#ifndef ZYNTEX_MAX_RESOLUTION
#define ZYNTEX_MAX_RESOLUTION 64
#endif

struct Texture {
    uint16_t pixels[ZYNTEX_MAX_RESOLUTION * ZYNTEX_MAX_RESOLUTION]{};
    uint16_t bufferLength = ZYNTEX_MAX_RESOLUTION * ZYNTEX_MAX_RESOLUTION;
    uint16_t resolution = ZYNTEX_MAX_RESOLUTION;

    bool loadFromFile(const char* fileName, bool overrideSize = false);
    uint16_t getPixel(uint16_t x, uint16_t y) const;
    void setPixel(uint16_t x, uint16_t y, uint16_t color);
    void clear(uint16_t color = 0);
};

// Legacy alias
using ZynTexture = Texture;

}  // namespace zyngine
