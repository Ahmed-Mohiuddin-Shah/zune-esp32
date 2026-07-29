#pragma once

#include <cstdint>

namespace zyngine::board {

struct PinMap {
    int homeButton = -1;
    int powerButton = -1;
    int batterySense = -1;
    int encoderClk = -1;
    int encoderDt = -1;
    int encoderButton = -1;
    int mouseClk = -1;
    int mouseData = -1;
    int sdSck = -1;
    int sdMiso = -1;
    int sdMosi = -1;
    int sdCs = -1;
    int touchYp = -1;
    int touchXm = -1;
    int touchYm = -1;
    int touchXp = -1;
    int backlight = -1;

    // Parallel ILI9486 bus
    int lcdWr = -1;
    int lcdRd = -1;
    int lcdRs = -1;
    int lcdCs = -1;
    int lcdD0 = -1;
    int lcdD1 = -1;
    int lcdD2 = -1;
    int lcdD3 = -1;
    int lcdD4 = -1;
    int lcdD5 = -1;
    int lcdD6 = -1;
    int lcdD7 = -1;
};

struct DisplayConfig {
    int width = 320;
    int height = 480;
    int rotation = 2;
};

struct BoardConfig {
    const char* name = "unknown";
    PinMap pins{};
    DisplayConfig display{};
};

}  // namespace zyngine::board
