#pragma once

#ifndef _CanSpeedAndRpmHandler_h
    #define _CanSpeedAndRpmHandler_h

#include "CanSpeedAndRpmStructs.h"
#include "CanMessageSender.h"

class CanSpeedAndRpmHandler
{
    const int CAN_SPEED_RPM_INTERVAL = 40;

    AbstractCanMessageSender *canMessageSender;

    unsigned long previousTime = millis();

    int Speed = 0;
    int Rpm = 0;

    public:
    CanSpeedAndRpmHandler(AbstractCanMessageSender * object)
    {
        canMessageSender = object;
    }

    void SetData(int speed, int rpm)
    {
        Speed = speed;
        Rpm = rpm;
    }

    void Process(unsigned long currentTime)
    {
        if (currentTime - previousTime > CAN_SPEED_RPM_INTERVAL)
        {
            previousTime = currentTime;

            if (Speed == 0xFFFF)
            {
                Speed = 0;
            }
            if (Rpm == 0xFFFF)
            {
                Rpm = 0;
            }

            CanSpeedAndRpmPacketSender speedAndRpmSender(canMessageSender);
            speedAndRpmSender.Send(Speed, Rpm);
        }
    }
};

#endif
