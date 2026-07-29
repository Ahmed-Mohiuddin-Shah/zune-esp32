#include "zyngine/hal/mutex.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

namespace zyngine::hal {

Mutex::Mutex() { impl_ = xSemaphoreCreateMutex(); }

Mutex::~Mutex() {
    if (impl_) {
        vSemaphoreDelete(static_cast<SemaphoreHandle_t>(impl_));
        impl_ = nullptr;
    }
}

void Mutex::lock() { xSemaphoreTake(static_cast<SemaphoreHandle_t>(impl_), portMAX_DELAY); }

void Mutex::unlock() { xSemaphoreGive(static_cast<SemaphoreHandle_t>(impl_)); }

}  // namespace zyngine::hal
