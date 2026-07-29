#include "clay.h"

#include "zyngine/core/engine.hpp"
#include "zyngine/graphics/color.hpp"
#include "zyngine/hal/encoder.hpp"
#include "zyngine/hal/input.hpp"
#include "zyngine/hal/log.hpp"
#include "zyngine/ui/clay/clay_host.hpp"
#include "zyngine/ui/focus_nav.hpp"
#include "zyngine/ui/themes/style_tokens.hpp"
#include "zyngine/ui/widgets/widgets.hpp"

#include <algorithm>
#include <cstdio>

using namespace zyngine;

enum class MenuScreen {
    Root,
    Options,
    Display,
    Network,
    Wifi,
    About,
};

class SmokeApp : public Engine {
public:
    void onUserCreate() override {
        ui::Theme::instance().setAmber();
        clay_.init(ui::ClayHostConfig{screenWidth_, screenHeight_, 512 * 1024, 256});
        hal::Log::info("Smoke", "scroll=encoder, MMB=select, RMB=theme");
    }

    void onUserUpdate(float deltaTime) override {
        auto ptr = hal::Input::pointer();
        auto enc = hal::Encoder::poll();

        static bool wasRight = false;
        bool rightDown = hal::Input::secondaryDown();
        if (rightDown && !wasRight) {
            green_ = !green_;
            if (green_) {
                ui::Theme::instance().setGreen();
            } else {
                ui::Theme::instance().setAmber();
            }
        }
        wasRight = rightDown;

        const auto& t = ui::Theme::instance().tokens();
        renderer_->clear(t.background);

        clay_.beginFrame(deltaTime, ptr.x, ptr.y, ptr.down, enc);

        char status[96];
        std::snprintf(status, sizeof(status), "focus:%d  %s%s",
                      ui::FocusNav::instance().focusIndex(),
                      enc.steps != 0 ? "TURN " : "", enc.down ? "BTN" : "");

        CLAY(CLAY_ID("Root"),
             {.layout = {.sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)},
                         .padding = CLAY_PADDING_ALL(12),
                         .childGap = 8,
                         .childAlignment = {},
                         .layoutDirection = CLAY_TOP_TO_BOTTOM},
              .backgroundColor = {0, 0, 0, 0}}) {
            ui::drawPanel("main", panelTitle(), [&]() {
                ui::drawLabel(green_ ? "PHOSPHOR: GREEN" : "PHOSPHOR: AMBER");
                ui::drawLabel(status);
                ui::drawLabel("Wheel=nav  MMB=ok  [..]=back");

                switch (screen_) {
                    case MenuScreen::Root:
                        drawRoot();
                        break;
                    case MenuScreen::Options:
                        drawOptions();
                        break;
                    case MenuScreen::Display:
                        drawDisplay();
                        break;
                    case MenuScreen::Network:
                        drawNetwork();
                        break;
                    case MenuScreen::Wifi:
                        drawWifi();
                        break;
                    case MenuScreen::About:
                        drawAbout();
                        break;
                }

                if (toast_[0]) {
                    ui::drawLabel(toast_);
                }
            });
        }

        auto cmds = clay_.endFrame();
        ui::renderClayCommands(cmds, *renderer_);

        renderer_->drawLine(static_cast<int>(ptr.x) - 6, static_cast<int>(ptr.y),
                            static_cast<int>(ptr.x) + 6, static_cast<int>(ptr.y), t.accent);
        renderer_->drawLine(static_cast<int>(ptr.x), static_cast<int>(ptr.y) - 6,
                            static_cast<int>(ptr.x), static_cast<int>(ptr.y) + 6, t.accent);
        renderer_->drawRect(4, 4, screenWidth_ - 8, screenHeight_ - 8, t.border);

        char fps[32];
        std::snprintf(fps, sizeof(fps), "%.0f FPS", deltaTime > 0.f ? 1.f / deltaTime : 0.f);
        const int fpsScale = std::max(1, t.fontSize / 14);
        renderer_->drawText(12, screenHeight_ - 12 - 8 * fpsScale, fps, t.dim, fpsScale);
    }

private:
    const char* panelTitle() const {
        switch (screen_) {
            case MenuScreen::Root:
                return "ZYNGINE // MAIN";
            case MenuScreen::Options:
                return "MAIN > OPTIONS";
            case MenuScreen::Display:
                return "OPTIONS > DISPLAY";
            case MenuScreen::Network:
                return "MAIN > NETWORK";
            case MenuScreen::Wifi:
                return "NETWORK > WIFI";
            case MenuScreen::About:
                return "MAIN > ABOUT";
        }
        return "ZYNGINE";
    }

    void go(MenuScreen next) {
        screen_ = next;
        ui::FocusNav::instance().setFocusIndex(0);
        toast_[0] = '\0';
    }

    void setToast(const char* msg) {
        std::snprintf(toast_, sizeof(toast_), "> %s", msg);
    }

    void drawRoot() {
        if (ui::drawButton("m_start", "[ START ]")) {
            setToast("START selected");
        }
        if (ui::drawButton("m_opt", "[ OPTIONS > ]")) {
            go(MenuScreen::Options);
        }
        if (ui::drawButton("m_net", "[ NETWORK > ]")) {
            go(MenuScreen::Network);
        }
        if (ui::drawButton("m_about", "[ ABOUT > ]")) {
            go(MenuScreen::About);
        }
        if (ui::drawButton("m_pwr", "[ POWER OFF ]")) {
            setToast("POWER OFF (demo)");
        }
    }

    void drawOptions() {
        if (ui::drawButton("o_back", "[ .. BACK ]")) {
            go(MenuScreen::Root);
        }
        if (ui::drawButton("o_disp", "[ DISPLAY > ]")) {
            go(MenuScreen::Display);
        }
        if (ui::drawButton("o_snd", "[ SOUND ]")) {
            setToast(soundOn_ ? "Sound: ON" : "Sound: OFF");
            soundOn_ = !soundOn_;
        }
        if (ui::drawButton("o_lang", "[ LANGUAGE ]")) {
            lang_ = (lang_ + 1) % 3;
            static const char* langs[] = {"EN", "JP", "DE"};
            setToast(langs[lang_]);
        }
    }

    void drawDisplay() {
        if (ui::drawButton("d_back", "[ .. BACK ]")) {
            go(MenuScreen::Options);
        }
        if (ui::drawButton("d_bright", "[ BRIGHTNESS ]")) {
            brightness_ = (brightness_ + 25) % 125;
            if (brightness_ == 0) {
                brightness_ = 25;
            }
            char buf[32];
            std::snprintf(buf, sizeof(buf), "Bright %d%%", brightness_);
            setToast(buf);
        }
        if (ui::drawButton("d_theme", "[ THEME TOGGLE ]")) {
            green_ = !green_;
            if (green_) {
                ui::Theme::instance().setGreen();
                setToast("Theme GREEN");
            } else {
                ui::Theme::instance().setAmber();
                setToast("Theme AMBER");
            }
        }
        if (ui::drawButton("d_invert", "[ INVERT ]")) {
            invert_ = !invert_;
            setToast(invert_ ? "Invert ON" : "Invert OFF");
        }
    }

    void drawNetwork() {
        if (ui::drawButton("n_back", "[ .. BACK ]")) {
            go(MenuScreen::Root);
        }
        if (ui::drawButton("n_wifi", "[ WIFI > ]")) {
            go(MenuScreen::Wifi);
        }
        if (ui::drawButton("n_bt", "[ BLUETOOTH ]")) {
            btOn_ = !btOn_;
            setToast(btOn_ ? "BT ON" : "BT OFF");
        }
        if (ui::drawButton("n_info", "[ IP INFO ]")) {
            setToast("192.168.1.42");
        }
    }

    void drawWifi() {
        if (ui::drawButton("w_back", "[ .. BACK ]")) {
            go(MenuScreen::Network);
        }
        if (ui::drawButton("w_scan", "[ SCAN ]")) {
            setToast("Scanning...");
        }
        if (ui::drawButton("w_ssid1", "[ zyngine-lab ]")) {
            setToast("Join zyngine-lab");
        }
        if (ui::drawButton("w_ssid2", "[ guest-ap ]")) {
            setToast("Join guest-ap");
        }
        if (ui::drawButton("w_off", "[ DISABLE WIFI ]")) {
            setToast("WiFi off");
        }
    }

    void drawAbout() {
        if (ui::drawButton("a_back", "[ .. BACK ]")) {
            go(MenuScreen::Root);
        }
        ui::drawLabel("Zyngine Stage 1");
        ui::drawLabel("Clay + FocusNav");
        ui::drawLabel("Encoder UI demo");
        if (ui::drawButton("a_ver", "[ VERSION ]")) {
            setToast("v0.1.0-smoke");
        }
        if (ui::drawButton("a_lic", "[ LICENSE ]")) {
            setToast("MIT-style / TBD");
        }
    }

    ui::ClayHost clay_;
    MenuScreen screen_ = MenuScreen::Root;
    bool green_ = false;
    bool soundOn_ = true;
    bool invert_ = false;
    bool btOn_ = false;
    int brightness_ = 75;
    int lang_ = 0;
    char toast_[40]{};
};

int main() {
    SmokeApp app;
    if (!app.initialize(1280, 720, 60)) {
        return 1;
    }
    app.run();
    return 0;
}
