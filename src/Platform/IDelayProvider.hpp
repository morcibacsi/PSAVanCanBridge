#pragma once

#include <cstdint>

class IDelayProvider
{
public:
    virtual ~IDelayProvider() = default;
    virtual void DelayMilliseconds(uint32_t milliseconds) = 0;
};
