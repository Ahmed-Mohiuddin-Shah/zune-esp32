#include "zyngine/graphics/texture.hpp"

#include "zyngine/graphics/color.hpp"
#include "zyngine/hal/filesystem.hpp"
#include "zyngine/hal/log.hpp"

#include <cstdio>
#include <cstring>

namespace zyngine {

bool Texture::loadFromFile(const char* fileName, bool /*overrideSize*/) {
    hal::File file;
    if (!file.open(fileName, hal::FileMode::Read)) {
        hal::Log::error("Texture", "Failed to open %s", fileName);
        return false;
    }

    char line[32];
    if (!file.readLine(line, sizeof(line))) {
        return false;
    }

    int x = 0;
    int y = 0;
    sscanf(line, "%d %d", &x, &y);
    if (x <= 0 || x > ZYNTEX_MAX_RESOLUTION) {
        hal::Log::error("Texture", "Invalid .zyntex resolution");
        return false;
    }

    bufferLength = static_cast<uint16_t>(x * x);
    resolution = static_cast<uint16_t>(x);

    int bufferIndex = 0;
    while (file.readLine(line, sizeof(line)) && bufferIndex < bufferLength) {
        unsigned color = 0;
        sscanf(line, "%u", &color);
        pixels[bufferIndex++] = static_cast<uint16_t>(color);
    }
    return true;
}

uint16_t Texture::getPixel(uint16_t x, uint16_t y) const {
    if (x < resolution && y < resolution) {
        return pixels[y * resolution + x];
    }
    return 0;
}

void Texture::setPixel(uint16_t x, uint16_t y, uint16_t color) {
    if (x < resolution && y < resolution) {
        pixels[y * resolution + x] = color;
    }
}

void Texture::clear(uint16_t color) {
    for (int i = 0; i < bufferLength; ++i) {
        pixels[i] = color;
    }
}

}  // namespace zyngine
