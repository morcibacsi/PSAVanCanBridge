// VanRadioRemoteHandler.h
#pragma once

#ifndef _VanRadioRemoteHandler_h
    #define _VanRadioRemoteHandler_h

#include "AbstractVanMessageHandler.h"

#include "VanDataToBridgeToCan.h"
#include "VanIgnitionDataToBridgeToCan.h"
#include "DoorStatus.h"

#include "CanTripInfoHandler.h"
#include "VanRadioRemoteStructs.h"

class VanRadioRemoteHandler : public AbstractVanMessageHandler {
    CanTripInfoHandler* canTripInfoHandler;
    CanRadioRemoteMessageHandler* canRadioRemoteMessageHandler;

    ~VanRadioRemoteHandler()
    {

    }

public:
    VanRadioRemoteHandler(CanTripInfoHandler* _canTripInfoHandler, CanRadioRemoteMessageHandler* _canRadioRemoteMessageHandler)
    {
        canTripInfoHandler = _canTripInfoHandler;
        canRadioRemoteMessageHandler = _canRadioRemoteMessageHandler;
    }

    bool ProcessMessage(
        const uint8_t identByte1,
        const uint8_t identByte2,
        const uint8_t vanMessageWithoutId[],
        const uint8_t messageLength,
        VanDataToBridgeToCan& dataToBridge,
        VanIgnitionDataToBridgeToCan& ignitionDataToBridge,
        DoorStatus& doorStatus) override
    {
        if (!IsVanIdent(identByte1, identByte2, VAN_ID_RADIO_REMOTE))
        {
            return false;
        }

        const VanRadioRemotePacket packet = DeSerialize<VanRadioRemotePacket>(vanMessageWithoutId);
        dataToBridge.RadioRemoteButton = packet.VanRadioRemotePacket[0];
        dataToBridge.RadioRemoteScroll = packet.VanRadioRemotePacket[1];

        canRadioRemoteMessageHandler->SetData(dataToBridge.RadioRemoteButton, dataToBridge.RadioRemoteScroll);

        if (packet.data.RemoteButton.seek_down_pressed && packet.data.RemoteButton.seek_up_pressed)
        {
            canTripInfoHandler->TripButtonPress();
        }

        return true;
    }
};

#endif
