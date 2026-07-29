#pragma once

namespace zyngine::hal {

enum class LogLevel {
    Debug,
    Info,
    Warn,
    Error,
};

struct Log {
    static void write(LogLevel level, const char* tag, const char* fmt, ...);
    static void debug(const char* tag, const char* fmt, ...);
    static void info(const char* tag, const char* fmt, ...);
    static void warn(const char* tag, const char* fmt, ...);
    static void error(const char* tag, const char* fmt, ...);
};

}  // namespace zyngine::hal
