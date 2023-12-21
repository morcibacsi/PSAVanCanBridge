// VanCarStatusPacketSender.h
#pragma once

#ifndef _VanCarStatusPacketSender_h
    #define _VanCarStatusPacketSender_h

#include "../IVanMessageSender.h"

class VanCarStatusPacketSender
{
    IVanMessageSender* vanMessageSender;

    public:
    VanCarStatusPacketSender(IVanMessageSender* object);

    bool GetCarStatus(uint8_t channelId);

    void Disable(uint8_t channelId);
};

#endif
