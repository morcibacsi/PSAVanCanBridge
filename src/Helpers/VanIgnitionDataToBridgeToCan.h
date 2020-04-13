// VanIgnitionDataToBridgeToCan.h
#pragma once

#ifndef _VanIgnitionDataToBridgeToCan_h
    #define _VanIgnitionDataToBridgeToCan_h

struct VanIgnitionDataToBridgeToCan
{
    int8_t OutsideTemperature = 0;
    int8_t WaterTemperature = 0;
    float InternalTemperature = 0;
    uint8_t EconomyModeActive = 0;
    uint8_t Ignition = 0;
    uint8_t DashboardLightingEnabled = 0;
    uint8_t NightMode = 0;
    uint8_t MileageByte1 = 0;
    uint8_t MileageByte2 = 0;
    uint8_t MileageByte3 = 0;
    uint8_t LeftStickButtonPressed = 0;
    uint8_t IsReverseEngaged = 0;
    uint8_t IsTrailerPresent = 0;
    uint8_t ExteriorRearLeftDistanceInCm = 0xFF;
    uint8_t ExteriorRearRightDistanceInCm = 0xFF;
    uint8_t InteriorRearLeftDistanceInCm = 0xFF;
    uint8_t InteriorRearRightDistanceInCm = 0xFF;
    uint8_t HaveDataFromParkingAid = 0;
};
#endif
