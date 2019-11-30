// VanDataToBridgeToCan.h
#pragma once

#ifndef _VanDataToBridgeToCan_h
    #define _VanDataToBridgeToCan_h

#include "LightStatus.h"
#include "DashIcons1.h"

struct VanDataToBridgeToCan
{
    int Speed = 0;
    int Rpm = 0;
    int Trip1Distance = 0;
    int Trip1Speed = 0;
    int Trip1Consumption = 0;
    int Trip2Distance = 0;
    int Trip2Speed = 0;
    int Trip2Consumption = 0;
    int FuelConsumption = 0;
    int FuelLeftToPump = 0;
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
};
#endif
