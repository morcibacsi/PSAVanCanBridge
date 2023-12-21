#pragma once

#ifndef _MessageHandler_2A5_h
    #define _MessageHandler_2A5_h

#include "ICanMessageHandler.h"
#include "../ICanMessageSender.h"
#include "../../../Config.h"
#include "../../Helpers/DataBroker.h"
#include <inttypes.h>

//2010 does not exists, look for ETAT_RADIO_TEXTE

/*
    Radio RDS data
*/
class MessageHandler_2A5 : public ICanMessageHandler
{
    Config* _config;

    public:
    MessageHandler_2A5(ICanMessageSender* object, DataBroker *dataBroker, Config* config) : ICanMessageHandler(object, dataBroker, 0x2A5, 8, 500-30) { _config = config; };

    void SetData(uint8_t letter1, uint8_t letter2, uint8_t letter3, uint8_t letter4, uint8_t letter5, uint8_t letter6, uint8_t letter7, uint8_t letter8);
    uint8_t SendMessage(unsigned long currentTime, bool forcedSend) override;
};
#endif
