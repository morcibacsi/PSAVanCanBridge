#pragma once

#include <cstdint>

class IDateTimeProvider
{
public:
    virtual ~IDateTimeProvider() = default;
    virtual void SetDateTime(
        uint16_t year,
        uint8_t month,
        uint8_t day,
        uint8_t hour,
        uint8_t minute,
        uint8_t second) = 0;
};
