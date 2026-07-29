#pragma once

#include "zyngine/hal/types.hpp"

namespace zyngine::hal {

struct Gpio {
    static void setMode(Pin pin, PinMode mode);
    static void write(Pin pin, PinLevel level);
    static PinLevel read(Pin pin);
};

}  // namespace zyngine::hal
