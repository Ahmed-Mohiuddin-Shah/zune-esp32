#pragma once

#include <cstddef>
#include <cstdint>

namespace zyngine::hal {

enum class FileMode {
    Read,
    Write,
    Append,
};

/// Platform-neutral file handle. Implementation lives in the platform layer.
class File {
public:
    File() = default;
    ~File();

    File(const File&) = delete;
    File& operator=(const File&) = delete;
    File(File&& other) noexcept;
    File& operator=(File&& other) noexcept;

    bool open(const char* path, FileMode mode);
    void close();
    bool isOpen() const;

    size_t read(void* buffer, size_t bytes);
    size_t write(const void* buffer, size_t bytes);
    bool readLine(char* buffer, size_t capacity);
    bool eof() const;

private:
    void* handle_ = nullptr;
};

struct Filesystem {
    static bool exists(const char* path);
};

}  // namespace zyngine::hal
