// VanRadioCommandPacketSender.h
#pragma once

#ifndef _VanRadioCommandPacketSender_h
    #define _VanRadioCommandPacketSender_h

#include "../IVanMessageSender.h"

class VanRadioCommandPacketSender
{
    IVanMessageSender* vanMessageSender;

    uint8_t _commandState;
    uint8_t _channelId;

    public:
    VanRadioCommandPacketSender(IVanMessageSender* object, uint8_t channelId);

    bool SetRadioState(uint8_t powerOn, uint8_t active, uint8_t keyboardEnabled, uint8_t autoVol, uint8_t loudness, uint8_t mute);

    bool SendSource(uint8_t source);

    bool SetKeyboardState(uint8_t enabled);

    bool SetAudioSettings(uint8_t exitOptions, uint8_t balance, uint8_t fader, uint8_t bass, uint8_t treble);

    void Disable();
};

#endif
