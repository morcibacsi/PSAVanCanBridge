// CanDash3MessageHandler.h
#pragma once

#ifndef _CanDash3MessageHandler_h
    #define _CanDash3MessageHandler_h

#include "AbstractCanMessageSender.h"
#include "CanMessageHandlerBase.h"
#include "CanDash3Structs.h"
#include "DashIcons1.h"

class CanDash3MessageHandler : public CanMessageHandlerBase
{
    static const uint8_t CAN_DASH3_MESSAGE_INTERVAL = 100;

    CanDash3PacketSender* Dash3Sender;

    DashIcons1 _DashIcons1;

    virtual void InternalProcess()
    {
        Dash3Sender->SendData(
            _DashIcons1.status.Handbrake,
            _DashIcons1.status.Mil,
            _DashIcons1.status.Abs,
            _DashIcons1.status.Esp,
            _DashIcons1.status.Airbag
        );
    }

    public:
    CanDash3MessageHandler(AbstractCanMessageSender* object) : CanMessageHandlerBase(object, CAN_DASH3_MESSAGE_INTERVAL)
    {
        Dash3Sender = new CanDash3PacketSender(object);
    }

    void SetData(
        DashIcons1 dashIcons1
    )
    {
        _DashIcons1 = dashIcons1;
    }
};

#endif
