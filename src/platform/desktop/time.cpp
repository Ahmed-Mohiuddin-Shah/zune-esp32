#include "zyngine/hal/time.hpp"

#include <chrono>
#include <thread>

namespace zyngine::hal {
namespace {
auto startTime() {
    static const auto t0 = std::chrono::steady_clock::now();
    return t0;
}
}  // namespace

uint64_t Time::millis() {
    using namespace std::chrono;
    return static_cast<uint64_t>(duration_cast<milliseconds>(steady_clock::now() - startTime()).count());
}

uint64_t Time::micros() {
    using namespace std::chrono;
    return static_cast<uint64_t>(duration_cast<microseconds>(steady_clock::now() - startTime()).count());
}

void Time::delayMs(uint32_t ms) { std::this_thread::sleep_for(std::chrono::milliseconds(ms)); }

void Time::delayUs(uint32_t us) { std::this_thread::sleep_for(std::chrono::microseconds(us)); }

}  // namespace zyngine::hal
