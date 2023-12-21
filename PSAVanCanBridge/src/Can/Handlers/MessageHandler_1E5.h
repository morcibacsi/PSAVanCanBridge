#pragma once

#ifndef _MessageHandler_1E5_h
    #define _MessageHandler_1E5_h

#include "ICanMessageHandler.h"
#include "../ICanMessageSender.h"
#include "../../../Config.h"
#include "../../Helpers/DataBroker.h"
#include <inttypes.h>

//2010 slight differences almost every byte

/*
    Radio status data for EMF (balance, fader, bass, treble, etc)
*/
class MessageHandler_1E5 : public ICanMessageHandler
{
    Config* _config;

    public:
    MessageHandler_1E5(ICanMessageSender* object, DataBroker *dataBroker, Config* config) : ICanMessageHandler(object, dataBroker, 0x1E5, 7, 500-30) { _config = config; };

    void SetData(uint8_t balance, uint8_t fader, uint8_t bass, uint8_t treble, uint8_t show_loudness, uint8_t loudness_on, uint8_t show_auto_vol, uint8_t auto_vol_on);
    uint8_t SendMessage(unsigned long currentTime, bool forcedSend) override;
};
#endif
