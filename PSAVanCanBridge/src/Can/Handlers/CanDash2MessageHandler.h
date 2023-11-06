// CanDash2MessageHandler.h
#pragma once

#ifndef _CanDash2MessageHandler_h
    #define _CanDash2MessageHandler_h

#include "../AbstractCanMessageSender.h"
#include "../Handlers/CanMessageHandlerBase.h"
#include "../Structs/CanDash2Structs.h"
#include "../../Helpers/LightStatus.h"
#include "../../Helpers/DashIcons1.h"
#include "../../Helpers/VanCanGearboxPositionMap.h"

class CanDash2MessageHandler : public CanMessageHandlerBase
{
    CanDash2PacketSender* Dash2Sender;

    uint8_t _ignition;
    LightStatus _lightStatus;
    DashIcons1 _dashIcons1;
    uint8_t _gearboxMode;
    uint8_t _gearboxSelection;
    uint8_t _gearboxPosition;

    VanCanGearboxPositionMap* _gearboxMap;

    virtual void InternalProcess()
    {
        Dash2Sender->SendData(
            _dashIcons1.status.SeatBeltWarning,
            _lightStatus.status.SideLights,
            _lightStatus.status.LowBeam,
            _lightStatus.status.HighBeam,
            _lightStatus.status.FrontFog,
            _lightStatus.status.RearFog,
            _lightStatus.status.LeftIndicator,
            _lightStatus.status.RightIndicator,
            _ignition,
            _dashIcons1.status.FuelLowLight,
            _dashIcons1.status.PassengerAirbag,
            0,
            0,
            0,
            _gearboxMode,
            _gearboxSelection,
            _gearboxPosition
        );
    }

    public:
    CanDash2MessageHandler(AbstractCanMessageSender* object) : CanMessageHandlerBase(object, CAN_ID_DASH2_INTERVAL)
    {
        Dash2Sender = new CanDash2PacketSender(object);
        _gearboxMap = new VanCanGearboxPositionMap();
    }

    void SetData(
        LightStatus lightStatus,
        DashIcons1 dashIcons1,
        uint8_t ignition,
        uint8_t gearboxMode,
        uint8_t gearboxSelection,
        uint8_t gearboxPosition
    )
    {
        _lightStatus = lightStatus;
        _dashIcons1 = dashIcons1;
        _ignition = ignition;

        _gearboxMode = _gearboxMap->GetGearboxModeFromVanMode(gearboxMode);
        _gearboxSelection = _gearboxMap->GetGearboxSelectionFromVanSelection(gearboxSelection);
        _gearboxPosition = _gearboxMap->GetGearboxPositionFromVanPosition(gearboxPosition);
    }
};
#endif
