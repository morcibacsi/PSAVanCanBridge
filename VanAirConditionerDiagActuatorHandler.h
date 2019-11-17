// VanAirConditionerDiagActuatorHandler.h
#pragma once

#ifndef _VanAirConditionerDiagActuatorHandler_h
    #define _VanAirConditionerDiagActuatorHandler_h

#include "AbstractVanMessageHandler.h"

#include "VanDataToBridgeToCan.h"
#include "VanIgnitionDataToBridgeToCan.h"
#include "DoorStatus.h"

#include "VanAirConditionerDiagStructs.h"
#include "CanAirConOnDisplayStructs.h"

class VanAirConditionerDiagActuatorHandler : public AbstractVanMessageHandler {
    ~VanAirConditionerDiagActuatorHandler()
    {

    }

private:
    uint8_t GetACDirection(uint8_t vanByte)
    {
        uint8_t result = CAN_AIRCON_DISPLAY_DIRECTION_AUTO;
        if (vanByte > 0x00)
        {
            result = CAN_AIRCON_DISPLAY_DIRECTION_UP;
        }
        if (vanByte > 0x1D)
        {
            result = CAN_AIRCON_DISPLAY_DIRECTION_UPDOWN;
        }
        if (vanByte > 0x2E)
        {
            result = CAN_AIRCON_DISPLAY_DIRECTION_DOWN;
        }
        if (vanByte > 0x3E)
        {
            result = CAN_AIRCON_DISPLAY_DIRECTION_FRONTDOWN;
        }
        if (vanByte > 0x60)
        {
            result = CAN_AIRCON_DISPLAY_DIRECTION_FRONT;
        }
        return result;
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
        if (!(IsVanIdent(identByte1, identByte2, VAN_ID_AIR_CONDITIONER_DIAG) && messageLength == 12 && vanMessageWithoutId[2] == VAN_ID_AIR_CONDITIONER_DIAG_ACTUATOR_STATUS))
        {
            return false;
        }

        const VanAirConditionerDiagActuatorStatusPacket packet = DeSerialize<VanAirConditionerDiagActuatorStatusPacket>(vanMessageWithoutId);

        dataToBridge.AirConDirection = GetACDirection(packet.data.DistributionStatus);

        return true;
    }
};

#endif
