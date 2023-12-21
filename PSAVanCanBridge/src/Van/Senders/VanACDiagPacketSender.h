// VanACDiagPacketSender.h
#pragma once

#ifndef _VanACDiagPacketSender_h
    #define _VanACDiagPacketSender_h

#include "../IVanMessageSender.h"

class VanACDiagPacketSender
{
    IVanMessageSender *vanMessageSender;

public:
    VanACDiagPacketSender(IVanMessageSender *object);

    void GetManufacturerInfo(uint8_t channelId);

    void GetSettings(uint8_t channelId);

    void GetSensorStatus(uint8_t channelId);

    void GetActuatorStatus(uint8_t channelId);

    void GetButtonStatus(uint8_t channelId);

    void GetFaultCodes(uint8_t channelId);

    void QueryAirConData(uint8_t channelId);

};
#endif
