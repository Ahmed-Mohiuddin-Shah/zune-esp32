#include "zyngine/hal/gpio.hpp"

#include "driver/gpio.h"

namespace zyngine::hal {
namespace {

gpio_mode_t toMode(PinMode mode) {
    switch (mode) {
        case PinMode::Input:
            return GPIO_MODE_INPUT;
        case PinMode::Output:
            return GPIO_MODE_OUTPUT;
        case PinMode::InputPullUp:
            return GPIO_MODE_INPUT;
        case PinMode::InputPullDown:
            return GPIO_MODE_INPUT;
    }
    return GPIO_MODE_INPUT;
}

}  // namespace

void Gpio::setMode(Pin pin, PinMode mode) {
    gpio_config_t cfg = {};
    cfg.pin_bit_mask = 1ULL << pin;
    cfg.mode = toMode(mode);
    cfg.pull_up_en = (mode == PinMode::InputPullUp) ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE;
    cfg.pull_down_en = (mode == PinMode::InputPullDown) ? GPIO_PULLDOWN_ENABLE : GPIO_PULLDOWN_DISABLE;
    cfg.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&cfg);
}

void Gpio::write(Pin pin, PinLevel level) {
    gpio_set_level(static_cast<gpio_num_t>(pin), level == PinLevel::High ? 1 : 0);
}

PinLevel Gpio::read(Pin pin) {
    return gpio_get_level(static_cast<gpio_num_t>(pin)) ? PinLevel::High : PinLevel::Low;
}

}  // namespace zyngine::hal
