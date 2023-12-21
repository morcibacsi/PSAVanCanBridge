// VanQueryRadioTuner.h
#pragma once

#ifndef _VanQueryRadioTuner_h
    #define _VanQueryRadioTuner_h

#include "VanMessageWriterBase.h"
#include "../IVanMessageSender.h"
#include "../Senders/VanRadioTunerPacketSender.h"

class VanQueryRadioTuner : public VanMessageWriterBase
{
    const static uint16_t QUERY_INTERVAL = 10000;
    const static uint8_t  CHANNEL = 8;

    uint8_t _ignition = 0;

    VanRadioTunerPacketSender *sender;

    virtual void InternalProcess() override;

    public:
    VanQueryRadioTuner(IVanMessageSender *vanMessageSender) : VanMessageWriterBase(vanMessageSender, QUERY_INTERVAL)
    {
        sender = new VanRadioTunerPacketSender(vanMessageSender);
    }

    void QueryNow();

    void SetData(uint8_t ignition);

    void Stop();
};

#endif
