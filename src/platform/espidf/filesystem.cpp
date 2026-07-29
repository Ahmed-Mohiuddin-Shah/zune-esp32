#include "zyngine/hal/filesystem.hpp"

#include "esp_log.h"

#include <cstdio>
#include <cstring>

// Stage 1: host-style FILE* over VFS (FATFS/SPIFFS can be mounted by the app).
// Full SD mount helper lives in display/board bring-up.

namespace zyngine::hal {

File::~File() { close(); }

File::File(File&& other) noexcept : handle_(other.handle_) { other.handle_ = nullptr; }

File& File::operator=(File&& other) noexcept {
    if (this != &other) {
        close();
        handle_ = other.handle_;
        other.handle_ = nullptr;
    }
    return *this;
}

bool File::open(const char* path, FileMode mode) {
    close();
    const char* m = "rb";
    switch (mode) {
        case FileMode::Read:
            m = "rb";
            break;
        case FileMode::Write:
            m = "wb";
            break;
        case FileMode::Append:
            m = "ab";
            break;
    }
    handle_ = std::fopen(path, m);
    return handle_ != nullptr;
}

void File::close() {
    if (handle_) {
        std::fclose(static_cast<FILE*>(handle_));
        handle_ = nullptr;
    }
}

bool File::isOpen() const { return handle_ != nullptr; }

size_t File::read(void* buffer, size_t bytes) {
    if (!handle_) return 0;
    return std::fread(buffer, 1, bytes, static_cast<FILE*>(handle_));
}

size_t File::write(const void* buffer, size_t bytes) {
    if (!handle_) return 0;
    return std::fwrite(buffer, 1, bytes, static_cast<FILE*>(handle_));
}

bool File::readLine(char* buffer, size_t capacity) {
    if (!handle_ || capacity == 0) return false;
    if (!std::fgets(buffer, static_cast<int>(capacity), static_cast<FILE*>(handle_))) {
        return false;
    }
    size_t n = std::strlen(buffer);
    if (n > 0 && (buffer[n - 1] == '\n' || buffer[n - 1] == '\r')) {
        buffer[n - 1] = '\0';
    }
    return true;
}

bool File::eof() const {
    if (!handle_) return true;
    return std::feof(static_cast<FILE*>(handle_)) != 0;
}

bool Filesystem::exists(const char* path) {
    FILE* f = std::fopen(path, "rb");
    if (!f) return false;
    std::fclose(f);
    return true;
}

}  // namespace zyngine::hal
