#include "zyngine/hal/display.hpp"
#include "zyngine/hal/encoder.hpp"
#include "zyngine/hal/input.hpp"
#include "zyngine/hal/log.hpp"
#include "zyngine/graphics/color.hpp"

#include <raylib.h>

#include <algorithm>
#include <cmath>
#include <vector>

namespace zyngine::hal {
namespace {

PointerState g_pointer{};
bool g_secondary = false;
float g_scrollX = 0.f;
float g_scrollY = 0.f;

EncoderState g_encoder{};
int g_encoderStepsAccum = 0;

class RaylibDisplay final : public Display {
public:
    bool begin(int width, int height) override {
        width_ = width;
        height_ = height;
        InitWindow(width, height, "Zyngine");
        SetTargetFPS(60);
        Image img = GenImageColor(width, height, BLACK);
        ImageFormat(&img, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
        texture_ = LoadTextureFromImage(img);
        UnloadImage(img);
        rgba_.assign(static_cast<size_t>(width * height * 4), 0);
        Log::info("Display", "Raylib display %dx%d (scroll=encoder, MMB=click)", width, height);
        return true;
    }

    void end() override {
        if (texture_.id != 0) {
            UnloadTexture(texture_);
            texture_ = {};
        }
        if (IsWindowReady()) {
            CloseWindow();
        }
    }

    int width() const override { return width_; }
    int height() const override { return height_; }

    void present(const uint16_t* pixels, int width, int height) override {
        for (int i = 0; i < width * height; ++i) {
            uint8_t r, g, b;
            zyngine::ZynColor::unpack(pixels[i], r, g, b);
            size_t o = static_cast<size_t>(i) * 4;
            rgba_[o + 0] = r;
            rgba_[o + 1] = g;
            rgba_[o + 2] = b;
            rgba_[o + 3] = 255;
        }
        UpdateTexture(texture_, rgba_.data());

        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexture(texture_, 0, 0, WHITE);
        EndDrawing();
    }

    bool shouldContinue() const override { return !WindowShouldClose(); }

    void pollEvents() override {
        Vector2 m = GetMousePosition();
        g_pointer.x = std::clamp(m.x, 0.f, static_cast<float>(width_ - 1));
        g_pointer.y = std::clamp(m.y, 0.f, static_cast<float>(height_ - 1));
        g_pointer.down = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
        g_pointer.pressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
        g_pointer.released = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
        g_secondary = IsMouseButtonDown(MOUSE_BUTTON_RIGHT);

        Vector2 wheel = GetMouseWheelMoveV();
        g_scrollX = wheel.x;
        g_scrollY = wheel.y;

        // Rotary encoder simulator:
        //   scroll down (+) -> next item, scroll up -> previous
        //   middle mouse    -> encoder button
        if (std::fabs(wheel.y) > 0.01f) {
            // Raylib: positive wheel.y = scroll up. Map to negative steps (previous).
            int notches = static_cast<int>(std::lround(-wheel.y));
            if (notches == 0) {
                notches = (wheel.y < 0.f) ? 1 : -1;
            }
            g_encoderStepsAccum += notches;
        }

        g_encoder.down = IsMouseButtonDown(MOUSE_BUTTON_MIDDLE);
        g_encoder.pressed = IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE);
        g_encoder.released = IsMouseButtonReleased(MOUSE_BUTTON_MIDDLE);
    }

private:
    int width_ = 0;
    int height_ = 0;
    Texture2D texture_{};
    std::vector<uint8_t> rgba_;
};

}  // namespace

Display* createDisplay() { return new RaylibDisplay(); }

PointerState Input::pointer() { return g_pointer; }

bool Input::secondaryDown() { return g_secondary; }

void Input::scrollDelta(float* dx, float* dy) {
    if (dx) *dx = g_scrollX;
    if (dy) *dy = g_scrollY;
    g_scrollX = 0.f;
    g_scrollY = 0.f;
}

EncoderState Encoder::poll() {
    EncoderState out = g_encoder;
    out.steps = g_encoderStepsAccum;
    g_encoderStepsAccum = 0;
    // Don't clear pressed/released here — they're frame-fresh from pollEvents.
    return out;
}

}  // namespace zyngine::hal
