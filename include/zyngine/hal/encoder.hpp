#pragma once

namespace zyngine::hal {

/// Rotary encoder state for the current frame.
/// Desktop simulator: mouse wheel = rotation, middle click = button.
struct EncoderState {
    /// Detents this frame. Positive = clockwise / next item; negative = previous.
    int steps = 0;
    bool down = false;
    bool pressed = false;   // edge: became down this frame
    bool released = false;  // edge: became up this frame
};

struct Encoder {
    /// Snapshot for this frame. `steps` is consumed (cleared) on read so callers
    /// don't double-apply the same wheel notches.
    static EncoderState poll();
};

}  // namespace zyngine::hal
