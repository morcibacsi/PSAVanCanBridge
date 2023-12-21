#pragma once

#ifndef _MessageHandler_325_h
    #define _MessageHandler_325_h

#include "ICanMessageHandler.h"
#include "../ICanMessageSender.h"
#include "../../../Config.h"
#include "../../Helpers/DataBroker.h"
#include <inttypes.h>

//2010 similar info, but different structure

/*
    CD related data for the EMF
*/
class MessageHandler_325 : public ICanMessageHandler
{
    Config* _config;

    public:
    MessageHandler_325(ICanMessageSender* object, DataBroker *dataBroker, Config* config) : ICanMessageHandler(object, dataBroker, 0x325, 3, 500-30) { _config = config; };

    void SetData(uint8_t cd1, uint8_t cd2, uint8_t error);
    uint8_t SendMessage(unsigned long currentTime, bool forcedSend) override;
};
#endif
