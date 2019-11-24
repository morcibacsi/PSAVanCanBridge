// VanInstrumentClusterHandlerV1.h
#pragma once

#ifndef _VanInstrumentClusterHandlerV1_h
    #define _VanInstrumentClusterHandlerV1_h

#include "../Handlers/AbstractVanMessageHandler.h"

#include "../../Helpers/VanDataToBridgeToCan.h"
#include "../../Helpers/VanIgnitionDataToBridgeToCan.h"
#include "../../Helpers/DoorStatus.h"

#include "../Structs/VanInstrumentClusterV1Structs.h"

class VanInstrumentClusterHandlerV1 : public AbstractVanMessageHandler {
    AbstractVanMessageHandler* vanInstrumentClusterHandlerV2;

    ~VanInstrumentClusterHandlerV1()
    {

    }

public:
    VanInstrumentClusterHandlerV1(AbstractVanMessageHandler* _vanInstrumentClusterHandlerV2)
    {
        vanInstrumentClusterHandlerV2 = _vanInstrumentClusterHandlerV2;
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
        if (!(IsVanIdent(identByte1, identByte2, VAN_ID_INSTRUMENT_CLUSTER_V1) && messageLength == 11))
        {
            return false;
        }

        uint8_t vanMessageV2[14] = { 0x00 };
        memcpy(vanMessageV2, vanMessageWithoutId, 11);

        return vanInstrumentClusterHandlerV2->ProcessMessage(identByte1, identByte2, vanMessageV2, 14, dataToBridge, ignitionDataToBridge, doorStatus);
    }
};

#endif
