// VanDashboardHandler.h
#pragma once

#ifndef _VanDashboardHandler_h
    #define _VanDashboardHandler_h

#include "../Handlers/AbstractVanMessageHandler.h"

#include "../../Helpers/VanDataToBridgeToCan.h"
#include "../../Helpers/VanIgnitionDataToBridgeToCan.h"
#include "../../Helpers/DoorStatus.h"

#include "../Structs/VanDashboardStructs.h"

class VanDashboardHandler : public AbstractVanMessageHandler {
    ~VanDashboardHandler()
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
        if (!(IsVanIdent(identByte1, identByte2, VAN_ID_DASHBOARD) && messageLength == 7))
        {
            return false;
        }

        const VanDashboardPacket packet = DeSerialize<VanDashboardPacket>(vanMessageWithoutId);
        ignitionDataToBridge.WaterTemperature = GetWaterTemperatureFromVANByte(packet.data.WaterTemperature.value);
        ignitionDataToBridge.OutsideTemperature = GetTemperatureFromVANByte(packet.data.ExternalTemperature.value);
        ignitionDataToBridge.EconomyModeActive = packet.data.Field1.economy_mode;
        ignitionDataToBridge.Ignition = packet.data.Field1.ignition_on || packet.data.Field1.accesories_on || packet.data.Field1.engine_running;
        ignitionDataToBridge.DashboardLightingEnabled = packet.data.Field0.is_backlight_off == 0;

        dataToBridge.LightStatuses.status.SideLights = packet.data.Field0.is_backlight_off == 0;
        dataToBridge.Ignition = ignitionDataToBridge.Ignition;

        ignitionDataToBridge.IsTrailerPresent = packet.data.Field1.trailer_present;
        ignitionDataToBridge.IsReverseEngaged = packet.data.Field1.reverse_gear;
        ignitionDataToBridge.MileageByte1 = packet.data.MileageByte1;
        ignitionDataToBridge.MileageByte2 = packet.data.MileageByte2;
        ignitionDataToBridge.MileageByte3 = packet.data.MileageByte3;

        return true;
    }
};

#endif
