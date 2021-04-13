// VanAirConditioner2Handler.h
#pragma once

#ifndef _VanAirConditioner2Handler_h
    #define _VanAirConditioner2Handler_h

#include "../Handlers/AbstractVanMessageHandler.h"

#include "../../Helpers/VanDataToBridgeToCan.h"
#include "../../Helpers/VanIgnitionDataToBridgeToCan.h"
#include "../../Helpers/DoorStatus.h"
#include "../../Helpers/Serializer.h"

#include "../Structs/VanAirConditioner2Structs.h"

class VanAirConditioner2Handler : public AbstractVanMessageHandler {
    ~VanAirConditioner2Handler()
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
        if (!(IsVanIdent(identByte1, identByte2, VAN_ID_AIR_CONDITIONER_2) && messageLength == VAN_ID_AIR_CONDITIONER_2_LENGTH))
        {
            return false;
        }

        const VanAirConditioner2Packet packet = DeSerialize<VanAirConditioner2Packet>(vanMessageWithoutId);
        if (dataToBridge->IsHeatingPanelPoweredOn == 1)
        {
            dataToBridge->IsAirConRunning = packet.data.Status1.ac_on && packet.data.Status1.ac_compressor_running;
            dataToBridge->IsWindowHeatingOn = packet.data.Status1.rear_window_heating_on;
        }

        if (HW_VERSION == 11 || !QUERY_AC_STATUS)
        {
            dataToBridge->InternalTemperature = GetEvaporatorTemperature(packet.data.EvaporatorTemperature);
        }

        return true;
    }
};

#endif
