// CanAirConOnDisplayHandler.h
#pragma once

#ifndef _CanAirConOnDisplayHandler_h
    #define _CanAirConOnDisplayHandler_h

#include "CanAirConOnDisplayStructs.h"
#include "CanMessageSender.h"

class CanAirConOnDisplayHandler
{
    AbstractCanMessageSender *canMessageSender;
    uint8_t FanSpeed;
    uint8_t FanSpeedChangedCounter;

    public:
    CanAirConOnDisplayHandler(AbstractCanMessageSender * object)
    {
        canMessageSender = object;
        FanSpeedChangedCounter = 0;
    }

    void SendCanAirConToDisplay(float temperatureLeft, float temperatureRight, uint8_t direction, uint8_t autoMode, uint8_t acOff, uint8_t off, uint8_t windshield, uint8_t fanSpeed, uint8_t recyclingOn)
    {
        if (FanSpeed != fanSpeed)
        {
            FanSpeedChangedCounter++;

            if (FanSpeedChangedCounter == 3)
            {
                FanSpeed = fanSpeed;
                FanSpeedChangedCounter = 0;
            }
        }
        else
        {
            FanSpeedChangedCounter = 0;
        }

        CanAirConditionOnDisplayPacketSender acSender(canMessageSender);
        acSender.SendACDataToDisplay(temperatureLeft, temperatureRight, direction, autoMode, acOff, off, windshield, FanSpeed, recyclingOn);
    }
};

#endif
