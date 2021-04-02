// VanInstrumentClusterHandlerV2.h
#pragma once

#ifndef _VanInstrumentClusterHandlerV2_h
    #define _VanInstrumentClusterHandlerV2_h

#include "../Handlers/AbstractVanMessageHandler.h"

#include "../../Helpers/VanDataToBridgeToCan.h"
#include "../../Helpers/VanIgnitionDataToBridgeToCan.h"
#include "../../Helpers/DoorStatus.h"
#include "../../Helpers/Serializer.h"

#include "../Structs/VanInstrumentClusterV2Structs.h"

class VanInstrumentClusterHandlerV2 : public AbstractVanMessageHandler {
    ~VanInstrumentClusterHandlerV2()
    {

    }

public:
    bool ProcessMessage(
        const uint8_t identByte1,
        const uint8_t identByte2,
        const uint8_t vanMessageWithoutId[],
        const uint8_t messageLength,
        VanDataToBridgeToCan *dataToBridge,
        VanIgnitionDataToBridgeToCan *ignitionDataToBridge,
        DoorStatus& doorStatus) override
    {
        if (!(IsVanIdent(identByte1, identByte2, VAN_ID_INSTRUMENT_CLUSTER_V2) && messageLength == 14))
        {
            return false;
        }

        const VanInstrumentClusterPacketV2 packet = DeSerialize<VanInstrumentClusterPacketV2>(vanMessageWithoutId);
        dataToBridge->LightStatuses.status.LowBeam = packet.data.LightsStatus.dipped_beam;
        dataToBridge->LightStatuses.status.HighBeam = packet.data.LightsStatus.high_beam;
        dataToBridge->LightStatuses.status.FrontFog = packet.data.LightsStatus.front_fog;
        dataToBridge->LightStatuses.status.RearFog = packet.data.LightsStatus.rear_fog;
        dataToBridge->LightStatuses.status.LeftIndicator = packet.data.LightsStatus.left_indicator;
        dataToBridge->LightStatuses.status.RightIndicator = packet.data.LightsStatus.right_indicator;
        dataToBridge->FuelLevel = packet.data.FuelLevel;
        dataToBridge->OilTemperature = GetOilTemperatureFromVANByteV2(packet.data.OilTemperature);

        ignitionDataToBridge->LowBeamOn = dataToBridge->LightStatuses.status.LowBeam;

        return true;
    }
};

#endif
