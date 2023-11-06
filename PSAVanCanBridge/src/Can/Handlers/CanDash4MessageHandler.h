// CanDash4MessageHandler.h
#pragma once

#ifndef _CanDash4MessageHandler_h
    #define _CanDash4MessageHandler_h

#include "../AbstractCanMessageSender.h"
#include "../Handlers/CanMessageHandlerBase.h"
#include "../Structs/CanDash4Structs.h"

class CanDash4MessageHandler : public CanMessageHandlerBase
{
    static const uint8_t CAN_DASH4_MESSAGE_INTERVAL = 100;

    CanDash4PacketSender* Dash4Sender;

    uint8_t _fuelLevel;
    int8_t _oilTemperature;

    virtual void InternalProcess()
    {
        Dash4Sender->SendData(
            _fuelLevel,
            _oilTemperature
        );
    }

    public:
    CanDash4MessageHandler(AbstractCanMessageSender* object) : CanMessageHandlerBase(object, CAN_DASH4_MESSAGE_INTERVAL)
    {
        Dash4Sender = new CanDash4PacketSender(object);
    }

    void SetData(uint8_t fuelLevel, int8_t oilTemperature)
    {
        _fuelLevel = fuelLevel;
        _oilTemperature = oilTemperature;
    }
};

#endif
