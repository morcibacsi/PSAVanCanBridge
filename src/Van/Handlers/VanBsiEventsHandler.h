// VanBsiEventsHandler.h
#pragma once

#ifndef _VanBsiEventsHandler_h
    #define _VanBsiEventsHandler_h

#include "../Handlers/AbstractVanMessageHandler.h"

#include "../../Helpers/VanDataToBridgeToCan.h"
#include "../../Helpers/VanIgnitionDataToBridgeToCan.h"
#include "../../Helpers/DoorStatus.h"

#include "../Structs/VanBsiEventsStructs.h"
#include "../../Can/Handlers/CanTripInfoHandler.h"

class VanBsiEventsHandler : public AbstractVanMessageHandler {
    const uint16_t chillTime = 1200;

    CanTripInfoHandler* _canTripInfoHandler;
    uint32_t lastTimeButtonPressed = 0;

    ~VanBsiEventsHandler()
    {

    }

    public:
    VanBsiEventsHandler(CanTripInfoHandler* canTripInfoHandler)
    {
        _canTripInfoHandler = canTripInfoHandler;
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
        if (!(IsVanIdent(identByte1, identByte2, VAN_ID_BSI_EVENTS) && messageLength == VAN_ID_BSI_EVENTS_LENGTH))
        {
            return false;
        }

        const VanBsiEventsPacket packet = DeSerialize<VanBsiEventsPacket>(vanMessageWithoutId);

        if (packet.data.Ident.event_source == VAN_BSI_EVENT_SOURCE_BSI)
        {
            if (packet.data.Cause.trip_button_pressed == 1)
            {
                uint32_t currentTime = millis();
                if (currentTime - lastTimeButtonPressed > chillTime)
                {
                    lastTimeButtonPressed = currentTime;
                    _canTripInfoHandler->TripButtonPress();
                }
            }
        }

        return true;
    }
};

#endif
