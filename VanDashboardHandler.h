// VanDashboardHandler.h
#pragma once

#ifndef _VanDashboardHandler_h
    #define _VanDashboardHandler_h

#include "AbstractVanMessageHandler.h"

#include "VanDataToBridgeToCan.h"
#include "VanIgnitionDataToBridgeToCan.h"
#include "DoorStatus.h"

#include "VanDashboardStructs.h"

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
        if (!IsVanIdent(identByte1, identByte2, VAN_ID_DASHBOARD))
        {
            return false;
        }

        const VanDashboardPacket packet = DeSerialize<VanDashboardPacket>(vanMessageWithoutId);
        ignitionDataToBridge.WaterTemperature = GetWaterTemperatureFromVANByte(packet.data.WaterTemperature.value);
        ignitionDataToBridge.OutsideTemperature = GetTemperatureFromVANByte(packet.data.ExternalTemperature.value);
        ignitionDataToBridge.EconomyModeActive = packet.data.Field1.economy_mode;
        ignitionDataToBridge.Ignition = packet.data.Field1.ignition_on || packet.data.Field1.accesories_on || packet.data.Field1.engine_running;
        ignitionDataToBridge.DashboardLightingEnabled = packet.VanDashboardPacket[0] != VAN_DASHBOARD_LIGHTS_OFF;

        dataToBridge.LightStatuses.status.SideLights = packet.VanDashboardPacket[0] != VAN_DASHBOARD_LIGHTS_OFF;
        dataToBridge.Ignition = ignitionDataToBridge.Ignition;
        ignitionDataToBridge.MileageByte1 = packet.data.MileageByte1;
        ignitionDataToBridge.MileageByte2 = packet.data.MileageByte2;
        ignitionDataToBridge.MileageByte3 = packet.data.MileageByte3;

        return true;
    }
};

#endif
