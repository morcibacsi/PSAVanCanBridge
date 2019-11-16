// VanIgnitionDataToBridgeToCan.h
#pragma once

#ifndef _VanIgnitionDataToBridgeToCan_h
    #define _VanIgnitionDataToBridgeToCan_h

struct VanIgnitionDataToBridgeToCan
{
    int OutsideTemperature = 0;
    int WaterTemperature = 0;
    uint8_t EconomyModeActive = 0;
    uint8_t Ignition = 0;
    uint8_t DashboardLightingEnabled = 0;
    uint8_t NightMode = 0;
    uint8_t MileageByte1 = 0;
    uint8_t MileageByte2 = 0;
    uint8_t MileageByte3 = 0;
    uint8_t LeftStickButtonPressed = 0;
};
#endif
