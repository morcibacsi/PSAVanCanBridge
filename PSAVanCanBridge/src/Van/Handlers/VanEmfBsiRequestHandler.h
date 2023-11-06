// VanEmfBsiRequestHandler.h
#pragma once

#ifndef _VanEmfBsiRequestHandler_h
    #define _VanEmfBsiRequestHandler_h

#include "../Handlers/AbstractVanMessageHandler.h"

#include "../../Helpers/VanDataToBridgeToCan.h"
#include "../../Helpers/VanIgnitionDataToBridgeToCan.h"
#include "../../Helpers/DoorStatus.h"
#include "../../Helpers/Serializer.h"

#include "../Structs/VanDisplayStatusStructs.h"
#include "../../Can/Handlers/CanTripInfoHandler.h"

class VanEmfBsiRequestHandler : public AbstractVanMessageHandler {
    CanTripInfoHandler* _canTripInfoHandler;

    ~VanEmfBsiRequestHandler()
    {

    }

    public:
    VanEmfBsiRequestHandler(CanTripInfoHandler* canTripInfoHandler)
    {
        _canTripInfoHandler = canTripInfoHandler;
    }

    bool ProcessMessage(
        const uint8_t identByte1,
        const uint8_t identByte2,
        const uint8_t vanMessageWithoutId[],
        const uint8_t messageLength,
        VanDataToBridgeToCan *dataToBridge,
        VanIgnitionDataToBridgeToCan *ignitionDataToBridge,
        DoorStatus& doorStatus) override
    {
        if (!(IsVanIdent(identByte1, identByte2, VAN_ID_DISPLAY_STATUS) && messageLength == VAN_ID_EMF_BSI_REQUEST_LENGTH))
        {
            return false;
        }

        const VanDisplayStatusPacket packet = DeSerialize<VanDisplayStatusPacket>(vanMessageWithoutId);

        if (packet.data.Requests.request_to_reset_course_totals)
        {
            _canTripInfoHandler->TripResetHappened();
        }

        return true;
    }
};

#endif
