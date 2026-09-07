#pragma once

#include <cstddef>
#include <cstdint>

class ISerial
{
public:
    virtual ~ISerial() = default;

    virtual int begin(int baudrate) = 0;
    virtual void end() = 0;

    virtual int available() = 0;
    virtual int read(uint8_t* buffer, size_t length, uint32_t timeout) = 0;
    virtual int readByte(uint8_t* byte, uint32_t timeout) = 0;

    virtual int write(const uint8_t* data, size_t length) = 0;
    virtual int writeByte(uint8_t byte) = 0;

    virtual void flush() = 0;
};
