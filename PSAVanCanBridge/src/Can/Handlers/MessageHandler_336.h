#pragma once

#ifndef _MessageHandler_336_h
    #define _MessageHandler_336_h

#include "ICanMessageHandler.h"
#include "../ICanMessageSender.h"
#include "../../Helpers/DataBroker.h"
#include "../../../Config.h"
#include <inttypes.h>

/*
    VIN for head unit
*/
class MessageHandler_336 : public ICanMessageHandler
{
    Config* _config;

    public:
    MessageHandler_336(ICanMessageSender* object, DataBroker *dataBroker, Config* config) : ICanMessageHandler(object, dataBroker, 0x336, 3, 1000-100) {
        _config = config;
    };

    uint8_t SendMessage(unsigned long currentTime, bool forcedSend) override;
};
#endif