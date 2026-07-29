#include "zyngine/hal/encoder.hpp"

namespace zyngine::hal {

EncoderState Encoder::poll() {
    // No hardware encoder wired in Stage 1 ESP-IDF smoke.
    return EncoderState{};
}

}  // namespace zyngine::hal
