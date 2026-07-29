#pragma once

#include <cstdint>

namespace zyngine::hal {

/// Abstract display present path. Graphics Core draws into an RGB565 buffer;
/// the platform Display flushes it to the physical (or simulated) panel.
class Display {
public:
    virtual ~Display() = default;

    virtual bool begin(int width, int height) = 0;
    virtual void end() = 0;

    virtual int width() const = 0;
    virtual int height() const = 0;

    /// Present a full-screen RGB565 framebuffer (row-major, width*height).
    virtual void present(const uint16_t* pixels, int width, int height) = 0;

    /// Optional dirty-rect present. Default presents the full buffer.
    virtual void presentRegion(const uint16_t* pixels, int width, int height,
                               int x, int y, int w, int h) {
        (void)x;
        (void)y;
        (void)w;
        (void)h;
        present(pixels, width, height);
    }

    virtual void setBacklight(bool on) { (void)on; }

    /// Return true while the window/session should keep running.
    virtual bool shouldContinue() const { return true; }

    /// Pump platform events (input, window). Called once per frame.
    virtual void pollEvents() {}
};

/// Factory implemented by the selected platform layer.
Display* createDisplay();

}  // namespace zyngine::hal
