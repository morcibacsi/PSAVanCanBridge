// VanAirConditionerDiagSensorHandler.h
#pragma once

#ifndef _VanAirConditionerDiagSensorHandler_h
    #define _VanAirConditionerDiagSensorHandler_h

#include "../Handlers/AbstractVanMessageHandler.h"

#include "../../Helpers/VanDataToBridgeToCan.h"
#include "../../Helpers/VanIgnitionDataToBridgeToCan.h"
#include "../../Helpers/DoorStatus.h"

#include "../Structs/VanAirConditionerDiagStructs.h"

class VanAirConditionerDiagSensorHandler : public AbstractVanMessageHandler {
    ~VanAirConditionerDiagSensorHandler()
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
        if (!(IsVanIdent(identByte1, identByte2, VAN_ID_AIR_CONDITIONER_DIAG) && messageLength == 22 && vanMessageWithoutId[2] == VAN_ID_AIR_CONDITIONER_DIAG_SENSOR_STATUS))
        {
            return false;
        }

        const VanAirConditionerDiagSensorStatusPacket packet = DeSerialize<VanAirConditionerDiagSensorStatusPacket>(vanMessageWithoutId);
        ignitionDataToBridge.InternalTemperature = GetACDiagTemperatureFromVanValue(packet.data.InternalTemperature1, packet.data.InternalTemperature2);
        dataToBridge.InternalTemperature = ignitionDataToBridge.InternalTemperature;
        return true;
    }
};

#endif
