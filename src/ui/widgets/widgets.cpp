#include "clay.h"

#include "zyngine/graphics/color.hpp"
#include "zyngine/ui/focus_nav.hpp"
#include "zyngine/ui/themes/style_tokens.hpp"
#include "zyngine/ui/widgets/widgets.hpp"

#include <cstring>

namespace zyngine::ui {
namespace {

Clay_Color toClay(uint16_t rgb565) {
    uint8_t r, g, b;
    ZynColor::unpack(rgb565, r, g, b);
    return Clay_Color{static_cast<float>(r), static_cast<float>(g), static_cast<float>(b), 255.f};
}

Clay_String clayStr(const char* s) {
    return Clay_String{false, static_cast<int32_t>(std::strlen(s)), s};
}

Clay_ElementId dynamicId(const char* id) { return Clay_GetElementId(clayStr(id)); }

uint16_t buttonBg(WidgetState state) {
    const auto& t = Theme::instance().tokens();
    switch (state) {
        case WidgetState::Pressed:
            return t.accent;
        case WidgetState::Hovered:
            return t.focus;
        case WidgetState::Disabled:
            return t.dim;
        case WidgetState::Normal:
        default:
            return t.panel;
    }
}

bool pointerDown() {
    Clay_PointerData ptr = Clay_GetPointerState();
    return ptr.state == CLAY_POINTER_DATA_PRESSED ||
           ptr.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME;
}

bool pointerReleasedThisFrame() {
    return Clay_GetPointerState().state == CLAY_POINTER_DATA_RELEASED_THIS_FRAME;
}

}  // namespace

void drawLabel(const char* text) {
    const auto& t = Theme::instance().tokens();
    CLAY_TEXT(clayStr(text),
              CLAY_TEXT_CONFIG({.textColor = toClay(t.foreground),
                               .fontSize = static_cast<uint16_t>(t.fontSize)}));
}

bool drawButton(const char* id, const char* text, bool disabled) {
    const auto& t = Theme::instance().tokens();
    bool clicked = false;

    const int slot = disabled ? -1 : FocusNav::instance().claimSlot();
    // Focus may update mid-frame when the pointer hovers this button.
    auto focusedNow = [&]() {
        return !disabled && slot >= 0 && FocusNav::instance().isFocused(slot);
    };

    CLAY(dynamicId(id), {
        .layout = {.sizing = {.width = CLAY_SIZING_GROW(0),
                              .height = CLAY_SIZING_FIXED(static_cast<float>(t.buttonHeight))},
                   .padding = CLAY_PADDING_ALL(static_cast<uint16_t>(t.padding)),
                   .childGap = 0,
                   .childAlignment = {.x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER},
                   .layoutDirection = CLAY_LEFT_TO_RIGHT},
        .backgroundColor = toClay([&]() -> uint16_t {
            if (disabled) {
                return buttonBg(WidgetState::Disabled);
            }
            if (Clay_Hovered() && slot >= 0) {
                FocusNav::instance().setFocusIndex(slot);
            }
            const bool hot = Clay_Hovered() || focusedNow();
            if (hot && pointerDown()) {
                return buttonBg(WidgetState::Pressed);
            }
            if (hot) {
                return buttonBg(WidgetState::Hovered);
            }
            return buttonBg(WidgetState::Normal);
        }()),
        .border =
            {.color = toClay((!disabled && (Clay_Hovered() || focusedNow())) ? t.focus : t.border),
             .width = {.left = focusedNow() ? uint16_t{2} : uint16_t{1},
                       .right = focusedNow() ? uint16_t{2} : uint16_t{1},
                       .top = focusedNow() ? uint16_t{2} : uint16_t{1},
                       .bottom = focusedNow() ? uint16_t{2} : uint16_t{1}}},
    }) {
        if (!disabled) {
            if (Clay_Hovered() && slot >= 0) {
                FocusNav::instance().setFocusIndex(slot);
            }
            if (Clay_Hovered() && pointerReleasedThisFrame()) {
                clicked = true;
            }
            if (focusedNow() && FocusNav::instance().takeActivate()) {
                clicked = true;
            }
        }

        const bool hot = !disabled && (Clay_Hovered() || focusedNow());
        const uint16_t fg =
            disabled ? t.dim : ((hot && pointerDown()) ? t.background : t.foreground);
        CLAY_TEXT(clayStr(text),
                  CLAY_TEXT_CONFIG({.textColor = toClay(fg),
                                   .fontSize = static_cast<uint16_t>(t.fontSize)}));
    }

    return clicked;
}

void drawPanel(const char* id, const char* title, const std::function<void()>& body) {
    const auto& t = Theme::instance().tokens();
    CLAY(dynamicId(id),
         {.layout = {.sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)},
                     .padding = CLAY_PADDING_ALL(static_cast<uint16_t>(t.padding)),
                     .childGap = static_cast<uint16_t>(t.gap),
                     .childAlignment = {},
                     .layoutDirection = CLAY_TOP_TO_BOTTOM},
          .backgroundColor = toClay(t.panel),
          .border = {.color = toClay(t.border),
                     .width = {.left = 1, .right = 1, .top = 1, .bottom = 1}}}) {
        if (title && title[0] != '\0') {
            CLAY_TEXT(clayStr(title),
                      CLAY_TEXT_CONFIG({.textColor = toClay(t.accent),
                                       .fontSize = static_cast<uint16_t>(t.fontSize)}));
        }
        if (body) {
            body();
        }
    }
}

}  // namespace zyngine::ui
