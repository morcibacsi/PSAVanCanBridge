#pragma once

#include <cstdint>

//Same order in AEE2004 0x220 do not change the order
union AirConditionerState
{
    struct {
        uint8_t IsHeatingPanelOn : 1;
        uint8_t IsRecyclingOn    : 1;
        uint8_t IsAirConEnabled  : 1;
        uint8_t IsACCompressorOn : 1;
        uint8_t IsWindowHeatingOn: 1;
        uint8_t FanSpeed         : 8;
        uint8_t                  : 3;
        uint8_t Direction        : 8;
        uint8_t Temperature      : 8;
    } data;
    uint32_t asByte;
};
