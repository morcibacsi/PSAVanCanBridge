// VanRadioSettingsPacketSender.h
#pragma once

#ifndef _VanRadioSettingsPacketSender_h
    #define _VanRadioSettingsPacketSender_h

#include "../IVanMessageSender.h"
#include "../Structs/VAN_4D4.h"

class VanRadioSettingsPacketSender
{
    IVanMessageSender* vanMessageSender;

    public:
    VanRadioSettingsPacketSender(IVanMessageSender* object);

    bool GetSettings(uint8_t channelId);

    void Disable(uint8_t channelId);
};

#endif
