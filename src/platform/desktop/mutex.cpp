#include "zyngine/hal/mutex.hpp"

#include <mutex>

namespace zyngine::hal {

Mutex::Mutex() { impl_ = new std::mutex(); }

Mutex::~Mutex() {
    delete static_cast<std::mutex*>(impl_);
    impl_ = nullptr;
}

void Mutex::lock() { static_cast<std::mutex*>(impl_)->lock(); }

void Mutex::unlock() { static_cast<std::mutex*>(impl_)->unlock(); }

}  // namespace zyngine::hal
