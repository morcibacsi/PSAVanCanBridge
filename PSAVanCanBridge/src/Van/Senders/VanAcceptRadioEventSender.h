// VanAcceptRadioEventSender.h
#pragma once

#ifndef _VanAcceptRadioEventSender_h
    #define _VanAcceptRadioEventSender_h

#include "../IVanMessageSender.h"

class VanAcceptRadioEventSender
{
    IVanMessageSender* vanMessageSender;

    public:
    VanAcceptRadioEventSender(IVanMessageSender* object);

    bool Accept(uint8_t channelId);

    void Disable(uint8_t channelId);
};

#endif
