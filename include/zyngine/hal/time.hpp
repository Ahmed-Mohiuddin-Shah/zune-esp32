#pragma once

#include <cstdint>

namespace zyngine::hal {

struct Time {
    /// Milliseconds since an arbitrary epoch (monotonic).
    static uint64_t millis();

    /// Microseconds since an arbitrary epoch (monotonic).
    static uint64_t micros();

    /// Block for at least `ms` milliseconds.
    static void delayMs(uint32_t ms);

    /// Block for at least `us` microseconds.
    static void delayUs(uint32_t us);
};

}  // namespace zyngine::hal
