// CanAirConOnDisplayHandlerOrig.h
#pragma once

#ifndef _CanAirConOnDisplayHandlerOrig_h
    #define _CanAirConOnDisplayHandlerOrig_h

#include "../Structs/CanAirConOnDisplayStructs.h"
#include "../AbstractCanMessageSender.h"

class CanAirConOnDisplayHandler
{
    const int CAN_AIRCON_INTERVAL = 100;
    const int CAN_AIRCON_FORCE_TIMEOUT = 3000;

    unsigned long previousTime = millis();
    unsigned long lastForceSentTime = 0;

    AbstractCanMessageSender *canMessageSender;
    uint8_t FanSpeed;
    uint8_t FanSpeedChangedCounter;

    float prevTemperatureLeft;
    float prevTemperatureRight;
    uint8_t prevDirection;
    uint8_t prevAutoMode;
    uint8_t prevAcOff; 
    uint8_t prevOff; 
    uint8_t prevWindshield; 
    uint8_t prevFanSpeed; 
    uint8_t prevRecyclingOn;

    public:
    CanAirConOnDisplayHandler(AbstractCanMessageSender * object)
    {
        canMessageSender = object;
        FanSpeedChangedCounter = 0;
    }

    void SendCanAirConToDisplay(unsigned long currentTime, float temperatureLeft, float temperatureRight, uint8_t direction, uint8_t autoMode, uint8_t acOff, uint8_t off, uint8_t windshield, uint8_t fanSpeed, uint8_t recyclingOn)
    {
        if (currentTime - previousTime > CAN_AIRCON_INTERVAL)
        {
            previousTime = currentTime;

            // add some tolerance on the fan speed to avoid 'flickering'
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

            // spare some bandwidth on CAN bus
            const bool sendMessageOnCan =
                prevTemperatureLeft != temperatureLeft || 
                prevTemperatureRight != temperatureRight || 
                prevDirection != direction || 
                prevAutoMode != autoMode || 
                prevAcOff != acOff || 
                prevOff != off || 
                prevWindshield != windshield || 
                prevFanSpeed != FanSpeed || 
                prevRecyclingOn != recyclingOn;

            if (sendMessageOnCan || (currentTime - lastForceSentTime) > CAN_AIRCON_FORCE_TIMEOUT)
            {
                lastForceSentTime = currentTime;

                prevTemperatureLeft = temperatureLeft;
                prevTemperatureRight = temperatureRight;
                prevDirection = direction;
                prevAutoMode = autoMode; 
                prevAcOff = acOff;
                prevOff = off;
                prevWindshield = windshield;
                prevFanSpeed = FanSpeed;
                prevRecyclingOn = recyclingOn;

                CanAirConditionOnDisplayPacketSender acSender(canMessageSender);
                acSender.SendACDataToDisplay(temperatureLeft, temperatureRight, direction, autoMode, acOff, off, windshield, FanSpeed, recyclingOn);
            }
        }
    }
};

#endif