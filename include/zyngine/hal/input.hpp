#pragma once

namespace zyngine::hal {

/// Pointer / touch state in screen pixel coordinates (top-left origin).
/// On desktop, the mouse simulates a single touch point.
struct PointerState {
    float x = 0.f;
    float y = 0.f;
    bool down = false;      // currently contacting (mouse button held / finger down)
    bool pressed = false;   // edge: became down this frame
    bool released = false;  // edge: became up this frame
};

struct Input {
    /// Current pointer / touch position in screen coordinates.
    /// Desktop: mouse position + left button as touch.
    static PointerState pointer();

    /// Secondary button (desktop right-click). Always false on embedded for now.
    static bool secondaryDown();

    /// Mouse wheel / scroll delta for the frame (desktop). Cleared after read.
    static void scrollDelta(float* dx, float* dy);
};

}  // namespace zyngine::hal
