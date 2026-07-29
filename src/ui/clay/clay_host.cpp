#define CLAY_IMPLEMENTATION
#include "clay.h"

#include "zyngine/ui/clay/clay_host.hpp"
#include "zyngine/ui/focus_nav.hpp"
#include "zyngine/ui/themes/style_tokens.hpp"
#include "zyngine/graphics/color.hpp"
#include "zyngine/hal/log.hpp"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <new>

namespace zyngine::ui {
namespace {

Clay_Dimensions measureText(Clay_StringSlice text, Clay_TextElementConfig* config, void* /*userData*/) {
    float size = config ? static_cast<float>(config->fontSize)
                        : static_cast<float>(Theme::instance().tokens().fontSize);
    // Monospace approximation matching Renderer::drawText (6px advance at scale 1 for 5x7).
    float scale = size / 7.f;
    if (scale < 1.f) scale = 1.f;
    return Clay_Dimensions{static_cast<float>(text.length) * 6.f * scale, 8.f * scale};
}

void handleClayError(Clay_ErrorData errorData) {
    zyngine::hal::Log::error("Clay", "%.*s", static_cast<int>(errorData.errorText.length),
                             errorData.errorText.chars ? errorData.errorText.chars : "");
}

uint16_t clayToRgb565(Clay_Color c) {
    return zyngine::ZynColor::pack(static_cast<uint8_t>(c.r), static_cast<uint8_t>(c.g),
                                   static_cast<uint8_t>(c.b));
}

}  // namespace

ClayHost::~ClayHost() { shutdown(); }

bool ClayHost::init(const ClayHostConfig& config) {
    shutdown();

    Clay_SetMaxElementCount(config.maxElements);
    const uint64_t needed = Clay_MinMemorySize();
    arenaBytes_ = config.arenaBytes > needed ? config.arenaBytes : static_cast<size_t>(needed);
    arenaMemory_ = std::malloc(arenaBytes_);
    if (!arenaMemory_) {
        return false;
    }

    Clay_Arena arena = Clay_CreateArenaWithCapacityAndMemory(arenaBytes_, arenaMemory_);
    Clay_Initialize(arena, Clay_Dimensions{static_cast<float>(config.screenWidth),
                                           static_cast<float>(config.screenHeight)},
                    Clay_ErrorHandler{handleClayError, nullptr});
    Clay_SetMeasureTextFunction(measureText, nullptr);
    zyngine::ui::Theme::instance().setScreenWidth(config.screenWidth);
    ready_ = true;
    return true;
}

void ClayHost::shutdown() {
    if (arenaMemory_) {
        std::free(arenaMemory_);
        arenaMemory_ = nullptr;
    }
    ready_ = false;
}

void ClayHost::beginFrame(float deltaTime, float pointerX, float pointerY, bool pointerDown,
                          const hal::EncoderState& encoder) {
    deltaTime_ = deltaTime;
    FocusNav::instance().beginFrame(encoder.steps, encoder.pressed);
    Clay_SetPointerState(Clay_Vector2{pointerX, pointerY}, pointerDown);
    Clay_BeginLayout();
}

Clay_RenderCommandArray ClayHost::endFrame() {
    Clay_RenderCommandArray cmds = Clay_EndLayout(deltaTime_);
    FocusNav::instance().endFrame();
    return cmds;
}

void renderClayCommands(const Clay_RenderCommandArray& commands, Renderer& renderer) {
    for (int32_t i = 0; i < commands.length; ++i) {
        Clay_RenderCommand* cmd = Clay_RenderCommandArray_Get(
            const_cast<Clay_RenderCommandArray*>(&commands), i);
        if (!cmd) {
            continue;
        }
        const auto& box = cmd->boundingBox;
        int x = static_cast<int>(box.x);
        int y = static_cast<int>(box.y);
        int w = static_cast<int>(box.width);
        int h = static_cast<int>(box.height);

        switch (cmd->commandType) {
            case CLAY_RENDER_COMMAND_TYPE_RECTANGLE: {
                renderer.fillRect(x, y, w, h, clayToRgb565(cmd->renderData.rectangle.backgroundColor));
                break;
            }
            case CLAY_RENDER_COMMAND_TYPE_TEXT: {
                Clay_StringSlice text = cmd->renderData.text.stringContents;
                char buf[256];
                size_t n = text.length < sizeof(buf) - 1 ? static_cast<size_t>(text.length)
                                                         : sizeof(buf) - 1;
                std::memcpy(buf, text.chars, n);
                buf[n] = '\0';
                int scale = std::max(1, static_cast<int>(cmd->renderData.text.fontSize / 7));
                renderer.drawText(x, y, buf, clayToRgb565(cmd->renderData.text.textColor), scale);
                break;
            }
            case CLAY_RENDER_COMMAND_TYPE_BORDER: {
                uint16_t c = clayToRgb565(cmd->renderData.border.color);
                auto& b = cmd->renderData.border;
                if (b.width.left) renderer.fillRect(x, y, b.width.left, h, c);
                if (b.width.right) renderer.fillRect(x + w - b.width.right, y, b.width.right, h, c);
                if (b.width.top) renderer.fillRect(x, y, w, b.width.top, c);
                if (b.width.bottom) renderer.fillRect(x, y + h - b.width.bottom, w, b.width.bottom, c);
                break;
            }
            case CLAY_RENDER_COMMAND_TYPE_SCISSOR_START: {
                renderer.setClipRect(x, y, w, h);
                break;
            }
            case CLAY_RENDER_COMMAND_TYPE_SCISSOR_END: {
                renderer.clearClipRect();
                break;
            }
            case CLAY_RENDER_COMMAND_TYPE_IMAGE:
            case CLAY_RENDER_COMMAND_TYPE_CUSTOM:
            case CLAY_RENDER_COMMAND_TYPE_NONE:
            default:
                break;
        }
    }
}

}  // namespace zyngine::ui
