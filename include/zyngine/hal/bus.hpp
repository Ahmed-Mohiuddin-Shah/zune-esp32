#pragma once

#include <cstddef>
#include <cstdint>

namespace zyngine::hal {

/// Placeholder SPI bus interface for future device drivers (SD already uses platform FS).
class SpiBus {
public:
    virtual ~SpiBus() = default;
    virtual bool begin(int sck, int miso, int mosi, int cs) = 0;
    virtual bool transfer(const uint8_t* tx, uint8_t* rx, size_t len) = 0;
};

/// Placeholder I2C bus interface for future IMU/RTC drivers.
class I2cBus {
public:
    virtual ~I2cBus() = default;
    virtual bool begin(int sda, int scl, uint32_t hz = 400000) = 0;
    virtual bool write(uint8_t addr, const uint8_t* data, size_t len) = 0;
    virtual bool read(uint8_t addr, uint8_t* data, size_t len) = 0;
};

}  // namespace zyngine::hal
