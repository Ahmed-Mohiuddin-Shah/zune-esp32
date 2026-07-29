#include "zyngine/hal/gpio.hpp"

#include "zyngine/hal/log.hpp"

namespace zyngine::hal {

void Gpio::setMode(Pin pin, PinMode mode) {
    (void)pin;
    (void)mode;
}

void Gpio::write(Pin pin, PinLevel level) {
    (void)pin;
    (void)level;
}

PinLevel Gpio::read(Pin pin) {
    (void)pin;
    return PinLevel::High;
}

}  // namespace zyngine::hal
