#pragma once

#include "zyngine/board/board.hpp"

namespace zyngine::board {

inline BoardConfig desktopBoard() {
    BoardConfig b;
    b.name = "desktop";
    b.display.width = 320;
    b.display.height = 480;
    b.display.rotation = 0;
    return b;
}

}  // namespace zyngine::board
