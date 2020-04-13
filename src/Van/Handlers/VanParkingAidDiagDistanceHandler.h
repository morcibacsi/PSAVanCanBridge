// VanParkingAidDiagDistanceHandler.h
#pragma once

#ifndef _VanParkingAidDiagDistanceHandler_h
    #define _VanParkingAidDiagDistanceHandler_h

#include "../Handlers/AbstractVanMessageHandler.h"

#include "../../Helpers/VanDataToBridgeToCan.h"
#include "../../Helpers/VanIgnitionDataToBridgeToCan.h"
#include "../../Helpers/DoorStatus.h"
#include "../../Helpers/Serializer.h"

#include "../Structs/VanParkingAidDiagStructs.h"

class VanParkingAidDiagDistanceHandler : public AbstractVanMessageHandler {
    static const uint16_t VAN_PARKING_AID_DATA_TIMEOUT = 3000;
    unsigned long _lastTimeDataArrived = 0;

    ~VanParkingAidDiagDistanceHandler()
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
        const unsigned long currentTime = millis();

        if (currentTime - _lastTimeDataArrived > VAN_PARKING_AID_DATA_TIMEOUT)
        {
            ignitionDataToBridge.ExteriorRearLeftDistanceInCm = 0xFF;
            ignitionDataToBridge.ExteriorRearRightDistanceInCm = 0xFF;

            ignitionDataToBridge.InteriorRearLeftDistanceInCm = 0xFF;
            ignitionDataToBridge.InteriorRearRightDistanceInCm = 0xFF;
            ignitionDataToBridge.HaveDataFromParkingAid = 0;
        }

        if (!(IsVanIdent(identByte1, identByte2, VAN_ID_PARKING_AID_DIAG_ANSWER) && messageLength == 24 && vanMessageWithoutId[2] == PR_DIAG_ANSWER_DISTANCE))
        {
            return false;
        }

        _lastTimeDataArrived = currentTime;

        const VanParkingAidDiagDistancePacket packet = DeSerialize<VanParkingAidDiagDistancePacket>(vanMessageWithoutId);

        ignitionDataToBridge.ExteriorRearLeftDistanceInCm = packet.data.ExteriorRearLeftDistanceInCm;
        ignitionDataToBridge.ExteriorRearRightDistanceInCm = packet.data.ExteriorRearRightDistanceInCm;

        ignitionDataToBridge.InteriorRearLeftDistanceInCm = packet.data.InteriorRearLeftDistanceInCm;
        ignitionDataToBridge.InteriorRearRightDistanceInCm = packet.data.InteriorRearRightDistanceInCm;
        ignitionDataToBridge.HaveDataFromParkingAid = 1;

        return true;
    }
};

#endif
