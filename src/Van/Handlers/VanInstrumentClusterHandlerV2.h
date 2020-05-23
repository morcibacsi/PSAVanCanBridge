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

    unsigned long _lightsOnTime = 0;
    uint8_t const CONSIDER_NIGHT_MODE_AFTER_SEC = 2;

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
        if (!(IsVanIdent(identByte1, identByte2, VAN_ID_INSTRUMENT_CLUSTER_V2) && messageLength == 14))
        {
            return false;
        }

        unsigned long currentTime = millis();

        const VanInstrumentClusterPacketV2 packet = DeSerialize<VanInstrumentClusterPacketV2>(vanMessageWithoutId);
        dataToBridge.LightStatuses.status.LowBeam = packet.data.LightsStatus.dipped_beam;
        dataToBridge.LightStatuses.status.HighBeam = packet.data.LightsStatus.high_beam;
        dataToBridge.LightStatuses.status.FrontFog = packet.data.LightsStatus.front_fog;
        dataToBridge.LightStatuses.status.RearFog = packet.data.LightsStatus.rear_fog;
        dataToBridge.LightStatuses.status.LeftIndicator = packet.data.LightsStatus.left_indicator;
        dataToBridge.LightStatuses.status.RightIndicator = packet.data.LightsStatus.right_indicator;
        dataToBridge.FuelLevel = packet.data.FuelLevel;
        dataToBridge.OilTemperature = GetOilTemperatureFromVANByteV2(packet.data.OilTemperature);

        if (dataToBridge.LightStatuses.status.LowBeam || dataToBridge.LightStatuses.status.HighBeam)
        {
            if (_lightsOnTime == 0)
            {
                _lightsOnTime = currentTime;
            }
        }
        else
        {
            _lightsOnTime = 0;
        }

        if (_lightsOnTime != 0 && currentTime - _lightsOnTime > CONSIDER_NIGHT_MODE_AFTER_SEC*1000)
        {
            ignitionDataToBridge.NightMode = 1;
        }
        else
        {
            ignitionDataToBridge.NightMode = 0;
        }

        return true;
    }
};

#endif
