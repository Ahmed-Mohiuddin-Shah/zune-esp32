#include "zyngine/hal/time.hpp"

#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "rom/ets_sys.h"

namespace zyngine::hal {

uint64_t Time::millis() { return static_cast<uint64_t>(esp_timer_get_time() / 1000); }

uint64_t Time::micros() { return static_cast<uint64_t>(esp_timer_get_time()); }

void Time::delayMs(uint32_t ms) { vTaskDelay(pdMS_TO_TICKS(ms)); }

void Time::delayUs(uint32_t us) { ets_delay_us(us); }

}  // namespace zyngine::hal
