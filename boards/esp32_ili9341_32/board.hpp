#pragma once

#include "zyngine/board/board.hpp"

namespace zyngine::board {

/// Classic ESP32 + 3.2" SPI ILI9341 + XPT2046 touch.
/// Pins match the TFT_eSPI User_Setup used with this hardware.
inline BoardConfig esp32Ili9341_32Board() {
    BoardConfig b;
    b.name = "esp32_ili9341_32";
    // Portrait 240x320 — tweak mirrorX/mirrorY here if still flipped.
    b.display.width = 240;
    b.display.height = 320;
    b.display.rotation = 0;
    b.display.mirrorX = true;
    b.display.mirrorY = false;

    b.pins.lcdSck = 18;
    b.pins.lcdMiso = 19;
    b.pins.lcdMosi = 23;
    b.pins.lcdCs = 15;
    b.pins.lcdDc = 2;
    b.pins.lcdRst = 4;
    b.pins.backlight = 17;
    b.pins.touchCs = 21;
    return b;
}

}  // namespace zyngine::board
