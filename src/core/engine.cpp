#include "zyngine/core/engine.hpp"

#include "zyngine/hal/log.hpp"
#include "zyngine/hal/time.hpp"

namespace zyngine {

bool Engine::initialize(int screenWidth, int screenHeight, int targetFPS) {
    screenWidth_ = screenWidth;
    screenHeight_ = screenHeight;
    targetFPS_ = targetFPS;
    this->screenWidth = screenWidth;
    this->screenHeight = screenHeight;

    display_ = hal::createDisplay();
    if (!display_ || !display_->begin(screenWidth, screenHeight)) {
        hal::Log::error("Engine", "Failed to create/begin display");
        return false;
    }

    // FB first (largest DRAM slab), then Clay in onUserCreate (other heap region).
    renderer_ = new Renderer(screenWidth, screenHeight, *display_);
    renderer = renderer_;
    if (!renderer_->ok()) {
        hal::Log::error("Engine", "Renderer framebuffer alloc failed");
        return false;
    }

    onUserCreate();
    return true;
}

void Engine::run() {
    uint64_t last = hal::Time::millis();
    while (display_ && display_->shouldContinue()) {
        display_->pollEvents();

        uint64_t now = hal::Time::millis();
        float deltaTime = static_cast<float>(now - last) / 1000.0f;
        last = now;
        if (deltaTime <= 0.f) {
            deltaTime = 1.0f / static_cast<float>(targetFPS_ > 0 ? targetFPS_ : 60);
        }

        renderer_->beginFrame();
        onUserUpdate(deltaTime);
        renderer_->endFrame();
    }

    if (display_) {
        display_->end();
    }
}

}  // namespace zyngine
