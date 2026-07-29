#include "zyngine/hal/log.hpp"

#include <cstdarg>
#include <cstdio>

namespace zyngine::hal {
namespace {

const char* levelName(LogLevel level) {
    switch (level) {
        case LogLevel::Debug:
            return "D";
        case LogLevel::Info:
            return "I";
        case LogLevel::Warn:
            return "W";
        case LogLevel::Error:
            return "E";
    }
    return "?";
}

void vwrite(LogLevel level, const char* tag, const char* fmt, va_list args) {
    std::fprintf(stderr, "[%s][%s] ", levelName(level), tag ? tag : "");
    std::vfprintf(stderr, fmt, args);
    std::fputc('\n', stderr);
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
