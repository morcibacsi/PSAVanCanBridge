#pragma once

#include <stdint.h>

enum class VanBusSpeed : uint32_t
{
    Kts62_5 = 62500u,
    Kts125 = 125000u,
};

namespace VanBusTiming
{
    constexpr bool IsSupported(VanBusSpeed speed)
    {
        return speed == VanBusSpeed::Kts62_5 || speed == VanBusSpeed::Kts125;
    }

    constexpr uint32_t BitRate(VanBusSpeed speed)
    {
        return static_cast<uint32_t>(speed);
    }

    // Round to the nearest LP-core cycle. RTC_FAST is calibrated by the HP CPU,
    // so neither bus speed depends on the oscillator's nominal frequency.
    constexpr uint32_t TimeSliceCycles(uint32_t lpClockHz, VanBusSpeed speed)
    {
        const uint32_t bitRate = BitRate(speed);
        return bitRate == 0 ? 0 : (lpClockHz + bitRate / 2u) / bitRate;
    }
}
