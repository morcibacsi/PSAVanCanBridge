// VanQueryParkingAid.h
#pragma once

#ifndef _VanQueryParkingAid_h
    #define _VanQueryParkingAid_h

#include "../AbstractVanMessageSender.h"
#include "../Structs/VanParkingAidDiagStructs.h"
#include "VanMessageWriterBase.h"

class VanQueryParkingAid : public VanMessageWriterBase
{
    const static uint16_t PARKING_AID_QUERY_INTERVAL = 120;

    const static uint8_t PARKING_AID_DIAG_START_CHANNEL = 5;
    const static uint8_t PARKING_AID_DIAG_QUERY_DISTANCE_CHANNEL = 6;
    const static uint8_t PARKING_AID_DIAG_DATA_CHANNEL = 7;

    uint8_t _ignition = 0;
    uint8_t _isReverseEngaged = 0;

    VanParkingAidDiagPacketSender* parkingAidDiagSender;

    virtual void InternalProcess() override
    {
        if (_ignition == 1 && _isReverseEngaged == 1)
        {
            parkingAidDiagSender->GetManufacturerInfo(PARKING_AID_DIAG_START_CHANNEL);
            parkingAidDiagSender->GetDistance(PARKING_AID_DIAG_QUERY_DISTANCE_CHANNEL);
            parkingAidDiagSender->QueryParkingRadarData(PARKING_AID_DIAG_DATA_CHANNEL);
        }
    }

    public:
        VanQueryParkingAid(AbstractVanMessageSender* vanMessageSender) : VanMessageWriterBase(vanMessageSender, PARKING_AID_QUERY_INTERVAL)
    {
            parkingAidDiagSender = new VanParkingAidDiagPacketSender(vanMessageSender);
    }

    void SetData(uint8_t ignition, uint8_t isReverseEngaged)
    {
        _ignition = ignition;
        _isReverseEngaged = isReverseEngaged;
    }

};

#endif
