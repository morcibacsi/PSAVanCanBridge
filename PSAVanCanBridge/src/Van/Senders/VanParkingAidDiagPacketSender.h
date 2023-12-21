// VanParkingAidDiagPacketSender.h
#pragma once

#ifndef _VanParkingAidDiagPacketSender_h
    #define _VanParkingAidDiagPacketSender_h

#include "../IVanMessageSender.h"

class VanParkingAidDiagPacketSender
{
    IVanMessageSender *vanMessageSender;

public:
    VanParkingAidDiagPacketSender(IVanMessageSender *object);

    void GetManufacturerInfo(uint8_t channelId);

    void GetDistance(uint8_t channelId);

    void GetConfiguration(uint8_t channelId);

    void GetInputStates(uint8_t channelId);

    void ActivateBuzzer(uint8_t channelId);

    void GetFaultCodes(uint8_t channelId);

    void FaultClearing(uint8_t channelId);

    void QueryParkingRadarData(uint8_t channelId);
};

#endif
