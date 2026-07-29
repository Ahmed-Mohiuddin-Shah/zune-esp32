#pragma once

#include "zyngine/ui/themes/style_tokens.hpp"

#include <functional>
#include <string>

namespace zyngine::ui {

enum class WidgetState {
    Normal,
    Hovered,
    Pressed,
    Disabled,
};

/// Emits a CLAY_TEXT label using the active theme.
void drawLabel(const char* text);

/// Themed button using Clay hit-testing (`Clay_Hovered` / pointer state).
/// Returns true if the button was activated this frame (pointer released over it).
/// Pass `disabled` to force the disabled look and ignore input.
bool drawButton(const char* id, const char* text, bool disabled = false);

/// Emits a bordered panel and runs `body` for child widgets inside the Clay scope.
void drawPanel(const char* id, const char* title, const std::function<void()>& body);

struct Label {
    std::string text;
    void build() const { drawLabel(text.c_str()); }
};

struct Button {
    std::string id = "btn";
    std::string text;
    bool disabled = false;
    bool clicked = false;
    void build() { clicked = drawButton(id.c_str(), text.c_str(), disabled); }
};

struct Panel {
    std::string id = "panel";
    std::string title;
    void build(const std::function<void()>& body) const { drawPanel(id.c_str(), title.c_str(), body); }
};

}  // namespace zyngine::ui
