#pragma once

#include <cstdint>
#include "Application/IApplicationClock.hpp"

class SimulatedClock final : public IApplicationClock
{
    uint64_t _timeMs = 0;

public:
    uint64_t NowMs() const override { return _timeMs; }
    void SetTimeMs(uint64_t timeMs) { _timeMs = timeMs; }
    void AdvanceTimeMs(uint64_t deltaMs) { _timeMs += deltaMs; }
};

