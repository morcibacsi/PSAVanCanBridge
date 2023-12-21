// VanQueryRadio.h
#pragma once

#ifndef _VanQueryRadio_h
    #define _VanQueryRadio_h

#include "VanMessageWriterBase.h"
#include "../IVanMessageSender.h"
#include "../Senders/VanRadioSettingsPacketSender.h"

class VanQueryRadioSettings : public VanMessageWriterBase
{
    const static uint16_t RADIO_SETTINGS_QUERY_INTERVAL = 900;
    const static uint8_t  RADIO_SETTINGS_CHANNEL = 8;

    uint8_t _ignition = 0;

    VanRadioSettingsPacketSender *radioSettingsSender;

    virtual void InternalProcess();

    public:

    VanQueryRadioSettings(IVanMessageSender *vanMessageSender) : VanMessageWriterBase(vanMessageSender, RADIO_SETTINGS_QUERY_INTERVAL)
    {
        radioSettingsSender = new VanRadioSettingsPacketSender(vanMessageSender);
    }

    void QueryNow();

    void SetData(uint8_t ignition);

    void Stop();
};

#endif
