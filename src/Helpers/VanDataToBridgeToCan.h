// VanDataToBridgeToCan.h
#pragma once

#ifndef _VanDataToBridgeToCan_h
    #define _VanDataToBridgeToCan_h

#include "LightStatus.h"
#include "DashIcons1.h"

struct VanDataToBridgeToCan
{
    uint8_t Speed = 0;
    uint16_t Rpm = 0;
    uint16_t Trip1Distance = 0;
    uint8_t Trip1Speed = 0;
    uint16_t Trip1Consumption = 0;
    uint16_t Trip2Distance = 0;
    uint8_t Trip2Speed = 0;
    uint16_t Trip2Consumption = 0;
    uint16_t FuelConsumption = 0;
    uint16_t FuelLeftToPump = 0;
    float InternalTemperature = 0;
    uint8_t RadioRemoteButton = 0;
    uint8_t RadioRemoteScroll = 0;
    uint8_t IsHeatingPanelPoweredOn = 0; // Displays off
    uint8_t IsAirConEnabled = 0;   // AC enabled, but should consider IsAirConRunning also
    uint8_t IsAirConRunning = 0;
    uint8_t IsWindowHeatingOn = 0;
    uint8_t IsAirRecyclingOn = 0;
    uint8_t AirConFanSpeed = 0;
    uint8_t Ignition = 0;
    uint8_t FuelLevel = 0;
    LightStatus LightStatuses;
    DashIcons1 DashIcons1Field;
    uint8_t LeftStickButtonPressed = 0;
    uint8_t AirConDirection = 0;
    uint8_t OilTemperature = 0;
    uint16_t RightWheelPosition = 0;
    uint16_t LeftWheelPosition = 0;
};
#endif
