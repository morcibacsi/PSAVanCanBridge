#pragma once

#ifndef _MessageHandler_165_h
    #define _MessageHandler_165_h

#include "ICanMessageHandler.h"
#include "../ICanMessageSender.h"
#include "../../../Config.h"
#include "../../Helpers/DataBroker.h"
#include <inttypes.h>

//2010 almost the same as 2004 has some extended information

/*
    Radio status for EMF
*/
class MessageHandler_165 : public ICanMessageHandler
{
    Config* _config;

    public:
    MessageHandler_165(ICanMessageSender* object, DataBroker *dataBroker, Config* config) : ICanMessageHandler(object, dataBroker, 0x165, 4, 100-30) { _config = config; };

    void SetSource(uint8_t source);
    void SetData(uint8_t on, uint8_t muted);
    uint8_t SendMessage(unsigned long currentTime, bool forcedSend) override;
};
#endif
