// VanAirConditioner1Handler.h
#pragma once

#ifndef _VanAirConditioner1Handler_h
    #define _VanAirConditioner1Handler_h

#include "AbstractVanMessageHandler.h"

#include "VanDataToBridgeToCan.h"
#include "VanIgnitionDataToBridgeToCan.h"
#include "DoorStatus.h"

#include "VanCanAirConditionerSpeedMap.h"
#include "VanAirConditioner1Structs.h"

class VanAirConditioner1Handler : public AbstractVanMessageHandler {
    VanCanAirConditionerSpeedMap* vanCanAirConditionerSpeedMap;

    ~VanAirConditioner1Handler()
    {

    }

public:
    VanAirConditioner1Handler(VanCanAirConditionerSpeedMap* _vanCanAirConditionerSpeedMap)
    {
        vanCanAirConditionerSpeedMap = _vanCanAirConditionerSpeedMap;
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
        if (!IsVanIdent(identByte1, identByte2, VAN_ID_AIR_CONDITIONER_1))
        {
            return false;
        }

        const VanAirConditioner1Packet packet = DeSerialize<VanAirConditioner1Packet>(vanMessageWithoutId);
        if (
            vanMessageWithoutId[0] == 0x00 && (packet.data.FanSpeed == 0x00)  // off
            || vanMessageWithoutId[0] == 0x00 && (packet.data.FanSpeed == 0x0E)  // off + rear window heating
            || vanMessageWithoutId[0] == 0x01 && (packet.data.FanSpeed == 0x0E)  // off + rear window heating toggle
            || vanMessageWithoutId[0] == 0x04 && (packet.data.FanSpeed == 0x00)  // off + recycle
            || vanMessageWithoutId[0] == 0x04 && (packet.data.FanSpeed == 0x0E)  // off + rear window heating + recycle
            || vanMessageWithoutId[0] == 0x05 && (packet.data.FanSpeed == 0x00)  // off + rear window heating + recycle toggle
            || vanMessageWithoutId[0] == 0x05 && (packet.data.FanSpeed == 0x0E)  // off + rear window heating + recycle toggle
            )
        {
            dataToBridge.IsHeatingPanelPoweredOn = 0;
            dataToBridge.IsAirConEnabled = 0;
            dataToBridge.IsAirConRunning = 0;
            dataToBridge.IsWindowHeatingOn = 0;
            dataToBridge.AirConFanSpeed = 0;
        }
        else
        {
            dataToBridge.IsHeatingPanelPoweredOn = 1;
            dataToBridge.IsAirConEnabled = packet.data.Status.aircon_on_if_necessary;
            dataToBridge.IsAirRecyclingOn = packet.data.Status.recycling_on;

            dataToBridge.AirConFanSpeed = vanCanAirConditionerSpeedMap->GetFanSpeedFromVANByte(packet.data.FanSpeed, dataToBridge.IsAirConEnabled, dataToBridge.IsWindowHeatingOn, dataToBridge.IsAirRecyclingOn);
        }

        return true;
    }
};

#endif
