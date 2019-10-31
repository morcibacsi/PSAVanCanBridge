// VanSpeedAndRpmHandler.h
#pragma once

#ifndef _VanSpeedAndRpmHandler_h
    #define _VanSpeedAndRpmHandler_h

#include "AbstractVanMessageHandler.h"

#include "VanDataToBridgeToCan.h"
#include "VanIgnitionDataToBridgeToCan.h"
#include "DoorStatus.h"

#include "VanSpeedAndRpmStructs.h"

class VanSpeedAndRpmHandler : public AbstractVanMessageHandler {
    ~VanSpeedAndRpmHandler()
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
        if (!IsVanIdent(identByte1, identByte2, VAN_ID_SPEED_RPM))
        {
            return false;
        }

        const VanSpeedAndRpmPacket packet = DeSerialize<VanSpeedAndRpmPacket>(vanMessageWithoutId);
        if (packet.data.Rpm.data == 0xFFFF)
        {
            dataToBridge.Rpm = 0;
            dataToBridge.Speed = 0;
        }
        else
        {
            dataToBridge.Rpm = GetRpmFromVanData(packet.data.Rpm.data);
            dataToBridge.Speed = GetSpeedFromVanData(packet.data.Speed.data);
        }

        return true;
    }
};

#endif
