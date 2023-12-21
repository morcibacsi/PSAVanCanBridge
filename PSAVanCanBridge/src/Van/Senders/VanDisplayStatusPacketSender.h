// VanDisplayStatusPacketSender.h
#pragma once

#ifndef _VanDisplayStatusPacketSender_h
    #define _VanDisplayStatusPacketSender_h

#include "../IVanMessageSender.h"

class VanDisplayStatusPacketSender
{
    IVanMessageSender *vanMessageSender;

public:
    VanDisplayStatusPacketSender(IVanMessageSender * object);

    void SendStatus(uint8_t channelId, uint8_t resetTotals, uint8_t resetCumulative);

    void Disable(uint8_t channelId);
};

#endif
