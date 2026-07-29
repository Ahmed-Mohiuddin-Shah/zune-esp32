#pragma once

#include <cstdint>

namespace zyngine::hal {

enum class PinMode : uint8_t {
    Input = 0,
    Output,
    InputPullUp,
    InputPullDown,
};

enum class PinLevel : uint8_t {
    Low = 0,
    High = 1,
};

using Pin = int;

struct Rect {
    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;
};

}  // namespace zyngine::hal
