// VanCanGearboxPositionMap.h
#pragma once

#ifndef _VanCanGearboxPositionMap_h
    #define _VanCanGearboxPositionMap_h

#include <Arduino.h>

class VanCanGearboxPositionMap
{
    private:
        uint8_t _gearBoxMap[16];
        uint8_t _gearBoxModeMap[8];
        uint8_t _gearBoxSelectionMap[2];

        void SetGearboxMapping();

        void SetGearboxModeMapping();

        void SetGearboxSelectionMapping();

    public:
        VanCanGearboxPositionMap();

        uint8_t GetGearboxPositionFromVanPosition(uint8_t gearboxPosition);

        uint8_t GetGearboxModeFromVanMode(uint8_t gearboxMode);

        uint8_t GetGearboxSelectionFromVanSelection(uint8_t gearboxSelection);
};

#endif
