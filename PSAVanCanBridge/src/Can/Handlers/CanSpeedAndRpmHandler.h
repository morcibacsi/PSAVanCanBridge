#pragma once

#ifndef _CanSpeedAndRpmHandler_h
    #define _CanSpeedAndRpmHandler_h

#include "../Handlers/CanMessageHandlerBase.h"
#include "../Structs/CanSpeedAndRpmStructs.h"
#include "../AbstractCanMessageSender.h"

class CanSpeedAndRpmHandler : public CanMessageHandlerBase
{
    static const int CAN_SPEED_RPM_INTERVAL = 40;

    uint8_t _speed = 0;
    uint16_t _rpm = 0;
    uint16_t _distance;

    CanSpeedAndRpmPacketSender* speedAndRpmSender;

    void InternalProcess() override
    {
        speedAndRpmSender->Send(_speed, _rpm, _distance);
    }

    public:
    CanSpeedAndRpmHandler(AbstractCanMessageSender * object) : CanMessageHandlerBase(object, CAN_SPEED_RPM_INTERVAL)
    {
        speedAndRpmSender = new CanSpeedAndRpmPacketSender(object);
    }

    void SetData(uint8_t speed, uint16_t rpm, uint16_t distance)
    {
        _speed = speed == 0xFF ? 0 : speed;
        _rpm = rpm == 0xFFFF ? 0 : rpm;
        _distance = distance;
    }
};

#endif
