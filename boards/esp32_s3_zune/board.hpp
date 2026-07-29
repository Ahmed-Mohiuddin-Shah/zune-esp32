#pragma once

#include "zyngine/board/board.hpp"

namespace zyngine::board {

/// Pin map for the Zune-like ESP32-S3 handheld (moved from legacy config_user.h).
/// Note: parallel LCD data pins overlap resistive-touch pins; touch requires
/// restoring pin modes after each read (same as the original design).
inline BoardConfig esp32S3ZuneBoard() {
    BoardConfig b;
    b.name = "esp32_s3_zune";
    b.display.width = 320;
    b.display.height = 480;
    b.display.rotation = 2;

    b.pins.homeButton = 3;
    b.pins.powerButton = 21;
    b.pins.batterySense = 1;
    b.pins.encoderClk = 18;
    b.pins.encoderDt = 17;
    b.pins.encoderButton = 43;
    b.pins.mouseClk = 45;
    b.pins.mouseData = 38;
    b.pins.sdSck = 36;
    b.pins.sdMiso = 37;
    b.pins.sdMosi = 35;
    b.pins.sdCs = 39;
    b.pins.touchYp = 14;
    b.pins.touchXm = 13;
    b.pins.touchYm = 5;
    b.pins.touchXp = 4;
    b.pins.backlight = 2;

    b.pins.lcdWr = 12;
    b.pins.lcdRd = 46;
    b.pins.lcdRs = 13;
    b.pins.lcdCs = 14;
    b.pins.lcdD0 = 4;
    b.pins.lcdD1 = 5;
    b.pins.lcdD2 = 6;
    b.pins.lcdD3 = 7;
    b.pins.lcdD4 = 15;
    b.pins.lcdD5 = 16;
    b.pins.lcdD6 = 10;
    b.pins.lcdD7 = 11;
    return b;
}

}  // namespace zyngine::board
