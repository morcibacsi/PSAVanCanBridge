#pragma once

#ifndef _CanNaviPositionHandler_h
    #define _CanNaviPositionHandler_h

#include "../Handlers/CanMessageHandlerBase.h"
#include "../Structs/CanNaviPositionStructs.h"
#include "../AbstractCanMessageSender.h"

class CanNaviPositionHandler : public CanMessageHandlerBase
{
    static const uint8_t CAN_SPEED_RPM_INTERVAL = CAN_ID_NAVI_POS_INTERVAL;

    uint16_t _leftWheel = 0;
    uint16_t _rightWheel = 0;

    CanNaviPositionPacketSender* packetSender;

    void InternalProcess() override
    {
        packetSender->Send(_rightWheel, _leftWheel);
    }

    public:
    CanNaviPositionHandler(AbstractCanMessageSender * object) : CanMessageHandlerBase(object, CAN_SPEED_RPM_INTERVAL)
    {
        packetSender = new CanNaviPositionPacketSender(object);
    }

    void SetData(uint16_t rightWheel, uint16_t leftWheel)
    {
        _rightWheel = rightWheel;
        _leftWheel = leftWheel;
    }
};

#endif
