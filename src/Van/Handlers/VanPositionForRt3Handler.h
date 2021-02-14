// VanPositionForRt3Handler.h
#pragma once

#ifndef _VanPositionForRt3Handler_h
    #define _VanPositionForRt3Handler_h

#include "../Handlers/AbstractVanMessageHandler.h"

#include "../../Helpers/VanDataToBridgeToCan.h"
#include "../../Helpers/VanIgnitionDataToBridgeToCan.h"
#include "../../Helpers/DoorStatus.h"

#include "../Structs/VanPositionForRt3Structs.h"
#include "../../Helpers/Serializer.h"

class VanPositionForRt3Handler : public AbstractVanMessageHandler {
    ~VanPositionForRt3Handler()
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
        if (!(IsVanIdent(identByte1, identByte2, VAN_ID_POSITION_FOR_RT3) && messageLength == VAN_ID_POSITION_FOR_RT3_LENGTH))
        {
            return false;
        }

        const VanPositionForRt3Packet packet = DeSerialize<VanPositionForRt3Packet>(vanMessageWithoutId);
        dataToBridge.RightWheelPosition = packet.data.RearRightAbsStatus.asRawValue;
        dataToBridge.LeftWheelPosition = packet.data.RearLeftAbsStatus.asRawValue;

        return true;
    }
};

#endif
