// VanRadioTunerPacketSender.h
#pragma once

#ifndef _VanRadioTunerPacketSender_h
    #define _VanRadioTunerPacketSender_h

#include "../IVanMessageSender.h"

class VanRadioTunerPacketSender
{
    IVanMessageSender* vanMessageSender;

    public:
    VanRadioTunerPacketSender(IVanMessageSender* object);

    bool GetTunerData(uint8_t channelId);

    void Disable(uint8_t channelId);
};

#endif
