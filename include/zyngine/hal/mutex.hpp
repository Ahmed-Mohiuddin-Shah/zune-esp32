#pragma once

namespace zyngine::hal {

/// Lightweight mutex. On single-threaded desktop this may be a no-op lock.
class Mutex {
public:
    Mutex();
    ~Mutex();

    Mutex(const Mutex&) = delete;
    Mutex& operator=(const Mutex&) = delete;

    void lock();
    void unlock();

    class Guard {
    public:
        explicit Guard(Mutex& m) : m_(m) { m_.lock(); }
        ~Guard() { m_.unlock(); }
        Guard(const Guard&) = delete;
        Guard& operator=(const Guard&) = delete;

    private:
        Mutex& m_;
    };

private:
    void* impl_ = nullptr;
};

}  // namespace zyngine::hal
