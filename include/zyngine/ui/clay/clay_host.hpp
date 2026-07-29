#pragma once

#include "zyngine/graphics/renderer.hpp"
#include "zyngine/hal/encoder.hpp"

#include <cstddef>
#include <cstdint>

struct Clay_RenderCommandArray;

namespace zyngine::ui {

struct ClayHostConfig {
    int screenWidth = 320;
    int screenHeight = 480;
    size_t arenaBytes = 256 * 1024;
    uint32_t maxElements = 256;
};

/// Owns Clay arena + context. Drives FocusNav from the rotary encoder each frame.
class ClayHost {
public:
    ClayHost() = default;
    ~ClayHost();

    ClayHost(const ClayHost&) = delete;
    ClayHost& operator=(const ClayHost&) = delete;

    bool init(const ClayHostConfig& config);
    void shutdown();

    /// pointer* = touch/mouse; encoder drives focus traversal (scroll/MMB on desktop).
    void beginFrame(float deltaTime, float pointerX, float pointerY, bool pointerDown,
                    const hal::EncoderState& encoder = {});
    Clay_RenderCommandArray endFrame();

    bool ready() const { return ready_; }

private:
    bool ready_ = false;
    void* arenaMemory_ = nullptr;
    size_t arenaBytes_ = 0;
    float deltaTime_ = 0.016f;
};

void renderClayCommands(const Clay_RenderCommandArray& commands, Renderer& renderer);

}  // namespace zyngine::ui
