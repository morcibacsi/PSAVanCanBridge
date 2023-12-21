// VanAcceptRadioEvent.h
#pragma once

#ifndef _VanAcceptRadioEvent_h
    #define _VanAcceptRadioEvent_h

#include "VanMessageWriterBase.h"
#include "../IVanMessageSender.h"
#include "../Senders/VanAcceptRadioEventSender.h"

class VanAcceptRadioEvent : public VanMessageWriterBase
{
    const static uint16_t QUERY_INTERVAL = 100;
    const static uint8_t  CHANNEL = 9;

    uint8_t _ignition = 0;

    VanAcceptRadioEventSender *sender;

    virtual void InternalProcess() override;

    public:

    VanAcceptRadioEvent(IVanMessageSender *vanMessageSender) : VanMessageWriterBase(vanMessageSender, QUERY_INTERVAL)
    {
        sender = new VanAcceptRadioEventSender(vanMessageSender);
    }

    void QueryNow();

    void SetData(uint8_t ignition);

    void Stop();
};

#endif
