#include "zyngine/hal/log.hpp"

#include <cstdarg>
#include <cstdio>

#include "esp_log.h"

namespace zyngine::hal {
namespace {

esp_log_level_t toEsp(LogLevel level) {
    switch (level) {
        case LogLevel::Debug:
            return ESP_LOG_DEBUG;
        case LogLevel::Info:
            return ESP_LOG_INFO;
        case LogLevel::Warn:
            return ESP_LOG_WARN;
        case LogLevel::Error:
            return ESP_LOG_ERROR;
    }
    return ESP_LOG_INFO;
}

void vwrite(LogLevel level, const char* tag, const char* fmt, va_list args) {
    char buf[256];
    vsnprintf(buf, sizeof(buf), fmt, args);
    ESP_LOG_LEVEL(toEsp(level), tag ? tag : "zyn", "%s", buf);
}

}  // namespace

void Log::write(LogLevel level, const char* tag, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vwrite(level, tag, fmt, args);
    va_end(args);
}

void Log::debug(const char* tag, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vwrite(LogLevel::Debug, tag, fmt, args);
    va_end(args);
}

void Log::info(const char* tag, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vwrite(LogLevel::Info, tag, fmt, args);
    va_end(args);
}

void Log::warn(const char* tag, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vwrite(LogLevel::Warn, tag, fmt, args);
    va_end(args);
}

void Log::error(const char* tag, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vwrite(LogLevel::Error, tag, fmt, args);
    va_end(args);
}

}  // namespace zyngine::hal
