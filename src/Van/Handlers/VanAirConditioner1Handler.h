// VanAirConditioner1Handler.h
#pragma once

#ifndef _VanAirConditioner1Handler_h
    #define _VanAirConditioner1Handler_h

#include "../../Helpers/VanDataToBridgeToCan.h"
#include "../../Helpers/VanIgnitionDataToBridgeToCan.h"
#include "../../Helpers/DoorStatus.h"
#include "../../Helpers/Serializer.h"
#include "../../Helpers/VanCanAirConditionerSpeedMap.h"

#include "../Handlers/AbstractVanMessageHandler.h"
#include "../Structs/VanAirConditioner1Structs.h"

class VanAirConditioner1Handler : public AbstractVanMessageHandler {
    VanCanAirConditionerSpeedMap* vanCanAirConditionerSpeedMap;

    const uint16_t SPEED_QUERY_SUPPRESS_TIME = 3500;

    uint8_t previousFanSpeed = 0;
    uint8_t prevACEnabled = 0;
    uint8_t prevWindowHeatingOn = 0;
    uint8_t prevAirRecycling = 0;

    unsigned long speedQuerySuppresedUntilTime = 0;
    unsigned long currentTime = 0;

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
        if (!(IsVanIdent(identByte1, identByte2, VAN_ID_AIR_CONDITIONER_1) && messageLength == 5))
        {
            return false;
        }

        currentTime = millis();

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

            const bool isModifierChanged =
                dataToBridge.IsAirConEnabled != prevACEnabled ||
                dataToBridge.IsWindowHeatingOn != prevWindowHeatingOn ||
                dataToBridge.IsAirRecyclingOn != prevAirRecycling;

            if (!isModifierChanged)
            {
                if (currentTime > speedQuerySuppresedUntilTime)
                {
                    previousFanSpeed = vanCanAirConditionerSpeedMap->GetFanSpeedFromVANByte(
                        packet.data.FanSpeed,
                        dataToBridge.IsAirConEnabled,
                        dataToBridge.IsWindowHeatingOn,
                        dataToBridge.IsAirRecyclingOn);
                }
            }
            else
            {
                speedQuerySuppresedUntilTime = currentTime + SPEED_QUERY_SUPPRESS_TIME;

                prevACEnabled = dataToBridge.IsAirConEnabled;
                prevWindowHeatingOn = dataToBridge.IsWindowHeatingOn;
                prevAirRecycling = dataToBridge.IsAirRecyclingOn;
            }

            dataToBridge.AirConFanSpeed = previousFanSpeed;
        }

        return true;
    }
};

#endif
