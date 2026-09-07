#pragma once

#include <cstdint>

class IApplicationClock
{
public:
    virtual ~IApplicationClock() = default;
    virtual uint64_t NowMs() const = 0;
};

