#pragma once

namespace zyngine {
class Renderer;

namespace hal {

class Display;

struct TouchCalData {
    int x0 = 200;  // raw at first target (after optional axis swap)
    int y0 = 200;
    int x1 = 3800;  // raw at second target
    int y1 = 3800;
    int sx0 = 0;  // screen pixel of first target
    int sy0 = 0;
    int sx1 = 0;  // screen pixel of second target
    int sy1 = 0;
    bool swapXy = true;  // XPT2046 axes often swapped vs screen
    bool valid = false;
};

/// Load/save touch cal in NVS; interactive 2-point wizard on ESP-IDF.
struct TouchCalib {
    static bool load(TouchCalData* out);
    static bool save(const TouchCalData& in);
    static void apply(const TouchCalData& cal);
    /// Blocking: draw targets, sample TL then BR, save + apply. No-op success on desktop.
    static bool run(Renderer& renderer, Display& display);
};

}  // namespace hal
}  // namespace zyngine
