#pragma once

#include "zyngine/graphics/color.hpp"

#include <algorithm>
#include <cstdint>

namespace zyngine::ui {

struct StyleTokens {
    uint16_t background = ZynColor::pack(8, 8, 4);
    uint16_t foreground = ZynColor::pack(255, 176, 0);  // amber
    uint16_t accent = ZynColor::pack(255, 200, 64);
    uint16_t dim = ZynColor::pack(120, 80, 0);
    uint16_t border = ZynColor::pack(200, 140, 0);
    uint16_t panel = ZynColor::pack(16, 12, 4);
    uint16_t focus = ZynColor::pack(255, 220, 120);
    uint16_t danger = ZynColor::pack(220, 60, 20);

    int fontSize = 14;
    int padding = 8;
    int gap = 6;
    int borderWidth = 1;
    int buttonHeight = 36;
};

inline StyleTokens retroAmberTokens() { return StyleTokens{}; }

inline StyleTokens retroGreenTokens() {
    StyleTokens t;
    t.background = ZynColor::pack(4, 12, 4);
    t.foreground = ZynColor::pack(80, 255, 80);
    t.accent = ZynColor::pack(140, 255, 140);
    t.dim = ZynColor::pack(20, 90, 20);
    t.border = ZynColor::pack(40, 180, 40);
    t.panel = ZynColor::pack(6, 20, 6);
    t.focus = ZynColor::pack(180, 255, 180);
    t.danger = ZynColor::pack(255, 80, 40);
    return t;
}

/// Active theme tokens (process-wide for Stage 1 simplicity).
class Theme {
public:
    static Theme& instance() {
        static Theme t;
        return t;
    }

    const StyleTokens& tokens() const { return tokens_; }

    /// Scale font/spacing from a 320px-wide reference design.
    void setScreenWidth(int width) {
        screenWidth_ = std::max(1, width);
        rescale();
    }

    int screenWidth() const { return screenWidth_; }

    void setAmber() {
        tokens_ = retroAmberTokens();
        rescale();
    }

    void setGreen() {
        tokens_ = retroGreenTokens();
        rescale();
    }

    void setTokens(const StyleTokens& t) {
        tokens_ = t;
        rescale();
    }

private:
    Theme() : tokens_(retroAmberTokens()) { rescale(); }

    void rescale() {
        // Reference layout designed for 320px-wide panels.
        constexpr int kRefW = 320;
        tokens_.fontSize = std::max(8, screenWidth_ * 14 / kRefW);
        tokens_.padding = std::max(4, screenWidth_ * 8 / kRefW);
        tokens_.gap = std::max(3, screenWidth_ * 6 / kRefW);
        tokens_.borderWidth = std::max(1, screenWidth_ * 1 / kRefW);
        tokens_.buttonHeight = std::max(24, screenWidth_ * 36 / kRefW);
    }

    StyleTokens tokens_;
    int screenWidth_ = 320;
};

}  // namespace zyngine::ui
