// CanDash2MessageHandler.h
#pragma once

#ifndef _CanDash2MessageHandler_h
    #define _CanDash2MessageHandler_h

#include "../AbstractCanMessageSender.h"
#include "../Handlers/CanMessageHandlerBase.h"
#include "../Structs/CanDash2Structs.h"
#include "../../Helpers/LightStatus.h"
#include "../../Helpers/DashIcons1.h"

class CanDash2MessageHandler : public CanMessageHandlerBase
{
    static const uint8_t CAN_DASH2_MESSAGE_INTERVAL = 100;

    CanDash2PacketSender* Dash2Sender;

    uint8_t _Ignition;
    LightStatus _LightStatus;
    DashIcons1 _DashIcons1;

    virtual void InternalProcess()
    {
        Dash2Sender->SendData(
            _DashIcons1.status.SeatBeltWarning,
            _LightStatus.status.SideLights,
            _LightStatus.status.LowBeam,
            _LightStatus.status.HighBeam,
            _LightStatus.status.FrontFog,
            _LightStatus.status.RearFog,
            _LightStatus.status.LeftIndicator,
            _LightStatus.status.RightIndicator,
            _Ignition,
            _DashIcons1.status.FuelLowLight,
            _DashIcons1.status.PassengerAirbag,
            0,
            0,
            0
        );
    }

    public:
    CanDash2MessageHandler(AbstractCanMessageSender* object) : CanMessageHandlerBase(object, CAN_DASH2_MESSAGE_INTERVAL)
    {
        Dash2Sender = new CanDash2PacketSender(object);
    }

    void SetData(
        LightStatus lightStatus,
        DashIcons1 dashIcons1,
        uint8_t ignition
    )
    {
        _LightStatus = lightStatus;
        _DashIcons1 = dashIcons1;
        _Ignition = ignition;
    }
};

#endif
