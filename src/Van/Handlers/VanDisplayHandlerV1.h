// VanDisplayHandler.h
#pragma once

#ifndef _VanDisplayHandler_h
    #define _VanDisplayHandler_h

#include "../Handlers/AbstractVanMessageHandler.h"

#include "../../Helpers/VanDataToBridgeToCan.h"
#include "../../Helpers/VanIgnitionDataToBridgeToCan.h"
#include "../../Helpers/DoorStatus.h"

#include "../Structs/VanDisplayStructsV1.h"

class VanDisplayHandlerV1 : public AbstractVanMessageHandler {
    AbstractVanMessageHandler* _vanDisplayHandlerV2;

    ~VanDisplayHandlerV1()
    {

    }

public:
    VanDisplayHandlerV1(AbstractVanMessageHandler* vanDisplayHandlerV2)
    {
        _vanDisplayHandlerV2 = vanDisplayHandlerV2;
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
        if (!(IsVanIdent(identByte1, identByte2, VAN_ID_DISPLAY_POPUP_V1) && messageLength == 14))
        {
            return false;
        }

        uint8_t vanMessageV2[16] = { 0x00 };
        memcpy(vanMessageV2, vanMessageWithoutId, 14);

        return _vanDisplayHandlerV2->ProcessMessage(identByte1, identByte2, vanMessageV2, 16, dataToBridge, ignitionDataToBridge, doorStatus);
    }
};

#endif
