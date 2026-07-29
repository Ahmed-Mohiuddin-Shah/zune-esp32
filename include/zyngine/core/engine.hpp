#pragma once

#include "zyngine/graphics/renderer.hpp"
#include "zyngine/hal/display.hpp"

namespace zyngine {

/// Application framework: create / update loop over a Display + Renderer.
class Engine {
public:
    Engine() = default;
    virtual ~Engine() = default;

    bool initialize(int screenWidth, int screenHeight, int targetFPS = 30);
    void run();

    virtual void onUserCreate() = 0;
    virtual void onUserUpdate(float deltaTime) = 0;

protected:
    int screenWidth_ = 0;
    int screenHeight_ = 0;
    int targetFPS_ = 30;
    Renderer* renderer_ = nullptr;
    hal::Display* display_ = nullptr;

    // Legacy accessors used by older samples
    Renderer* renderer = nullptr;
    int screenWidth = 0;
    int screenHeight = 0;
};

using Zyngine = Engine;

}  // namespace zyngine
