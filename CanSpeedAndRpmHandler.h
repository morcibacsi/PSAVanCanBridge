#pragma once

#ifndef _CanSpeedAndRpmHandler_h
    #define _CanSpeedAndRpmHandler_h

#include "CanMessageHandlerBase.h"
#include "CanSpeedAndRpmStructs.h"
#include "AbstractCanMessageSender.h"

class CanSpeedAndRpmHandler : public CanMessageHandlerBase
{
    static const int CAN_SPEED_RPM_INTERVAL = 40;

    int _Speed = 0;
    int _Rpm = 0;

    CanSpeedAndRpmPacketSender* speedAndRpmSender;

    void InternalProcess() override
    {
        speedAndRpmSender->Send(_Speed, _Rpm);
    }

    public:
    CanSpeedAndRpmHandler(AbstractCanMessageSender * object) : CanMessageHandlerBase(object, CAN_SPEED_RPM_INTERVAL)
    {
        speedAndRpmSender = new CanSpeedAndRpmPacketSender(object);
    }

    void SetData(int speed, int rpm)
    {
        _Speed = speed == 0xFFFF ? 0 : speed;
        _Rpm = rpm == 0xFFFF ? 0 : rpm;
    }
};

#endif
