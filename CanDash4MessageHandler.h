// CanDash4MessageHandler.h
#pragma once

#ifndef _CanDash4MessageHandler_h
    #define _CanDash4MessageHandler_h

#include "AbstractCanMessageSender.h"
#include "CanMessageHandlerBase.h"
#include "CanDash4Structs.h"

class CanDash4MessageHandler : public CanMessageHandlerBase
{
    static const uint8_t CAN_DASH4_MESSAGE_INTERVAL = 100;

    CanDash4PacketSender* Dash4Sender;

    uint8_t _FuelLevel;

    virtual void InternalProcess()
    {
        Dash4Sender->SendData(
            _FuelLevel
        );
    }

    public:
    CanDash4MessageHandler(AbstractCanMessageSender* object) : CanMessageHandlerBase(object, CAN_DASH4_MESSAGE_INTERVAL)
    {
        Dash4Sender = new CanDash4PacketSender(object);
    }

    void SetData(uint8_t fuelLevel)
    {
        _FuelLevel = fuelLevel;
    }
};

#endif
