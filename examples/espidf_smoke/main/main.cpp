#include "zyngine/core/engine.hpp"
#include "zyngine/graphics/color.hpp"
#include "zyngine/hal/log.hpp"
#include "zyngine/hal/time.hpp"
#include "zyngine/ui/clay/clay_host.hpp"
#include "zyngine/ui/themes/style_tokens.hpp"
#include "zyngine/ui/widgets/widgets.hpp"

#include "clay.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

using namespace zyngine;

class EspSmoke : public Engine {
public:
    void onUserCreate() override {
        ui::Theme::instance().setAmber();
        clay_.init(ui::ClayHostConfig{screenWidth_, screenHeight_, 256 * 1024, 128});
        hal::Log::info("Smoke", "ESP-IDF smoke ready");
    }

    void onUserUpdate(float deltaTime) override {
        const auto& t = ui::Theme::instance().tokens();
        renderer_->clear(t.background);

        clay_.beginFrame(deltaTime, 0.f, 0.f, false);
        CLAY(CLAY_ID("Root"),
             {.layout = {.sizing = {.width = CLAY_SIZING_GROW(0), .height = CLAY_SIZING_GROW(0)},
                         .padding = CLAY_PADDING_ALL(10),
                         .childGap = 8,
                         .childAlignment = {},
                         .layoutDirection = CLAY_TOP_TO_BOTTOM},
              .backgroundColor = {0, 0, 0, 0}}) {
            ui::drawPanel("main", "ESP-IDF // ZYNGINE", [&]() {
                ui::drawLabel("Pure IDF backend");
                ui::drawButton("ok", "[ OK ]", ui::WidgetState::Normal);
            });
        }
        auto cmds = clay_.endFrame();
        ui::renderClayCommands(cmds, *renderer_);

        renderer_->fillRect(20, 400, 100, 40, t.accent);
        renderer_->drawText(24, 412, "CORE", t.background, 2);
    }

private:
    ui::ClayHost clay_;
};

extern "C" void app_main(void) {
    EspSmoke app;
    if (!app.initialize(320, 480, 30)) {
        hal::Log::error("Smoke", "init failed");
        return;
    }
    // Engine::run never returns on ESP (shouldContinue always true)
    app.run();
}
