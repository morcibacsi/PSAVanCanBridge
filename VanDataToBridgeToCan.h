// VanDataToBridgeToCan.h
#pragma once

#ifndef _VanDataToBridgeToCan_h
    #define _VanDataToBridgeToCan_h

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
    int InternalTemperature = 0;
    uint8_t RadioRemoteButton = 0;
    uint8_t RadioRemoteScroll = 0;
    uint8_t IsHeatingPanelPoweredOn = 0; // Displays off
    uint8_t IsAirConEnabled = 0;   // AC enabled, but should consider IsAirConRunning also
    uint8_t IsAirConRunning = 0;
    uint8_t IsWindowHeatingOn = 0;
    uint8_t IsAirRecyclingOn = 0;
    uint8_t AirConFanSpeed = 0;
    uint8_t SeatBeltWarning = 0;
    uint8_t LowBeam = 0;
    uint8_t HighBeam = 0;
    uint8_t FrontFog = 0;
    uint8_t RearFog = 0;
    uint8_t LeftIndicator = 0;
    uint8_t RightIndicator = 0;
    uint8_t SideLights = 0;
    uint8_t Ignition = 0;
    uint8_t FuelLowLight = 0;
    uint8_t PassengerAirbag = 0;
    uint8_t Handbrake = 0;
    uint8_t Abs = 0;
    uint8_t Esp = 0;
    uint8_t Mil = 0;
    uint8_t Airbag = 0;
};
#endif
