// VanParkingAidDiagInputStateHandler.h
#pragma once

#ifndef _VanParkingAidDiagInputStateHandler_h
    #define _VanParkingAidDiagInputStateHandler_h

#include "../Handlers/AbstractVanMessageHandler.h"

#include "../../Helpers/VanDataToBridgeToCan.h"
#include "../../Helpers/VanIgnitionDataToBridgeToCan.h"
#include "../../Helpers/DoorStatus.h"
#include "../../Helpers/Serializer.h"

#include "../Structs/VanParkingAidDiagStructs.h"

class VanParkingAidDiagInputStateHandler : public AbstractVanMessageHandler {
    ~VanParkingAidDiagInputStateHandler()
    {

    }

public:
    bool ProcessMessage(
        const uint8_t identByte1,
        const uint8_t identByte2,
        const uint8_t vanMessageWithoutId[],
        const uint8_t messageLength,
        VanDataToBridgeToCan& dataToBridge,
        VanIgnitionDataToBridgeToCan& ignitionDataToBridge,
        DoorStatus& doorStatus) override
    {
        if (!(IsVanIdent(identByte1, identByte2, VAN_ID_PARKING_AID_DIAG_ANSWER) && messageLength == 5 && vanMessageWithoutId[2] == PR_DIAG_ANSWER_STATE_OF_INPUT))
        {
            return false;
        }

        const VanParkingAidInputStatePacket packet = DeSerialize<VanParkingAidInputStatePacket>(vanMessageWithoutId);

        ignitionDataToBridge.IsReverseEngaged = packet.data.Status.system_active;
        ignitionDataToBridge.IsTrailerPresent = packet.data.Status.trailer_present;

        return true;
    }
};

#endif
