#pragma once

#ifndef _MessageHandler_2B6_h
    #define _MessageHandler_2B6_h

#include "ICanMessageHandler.h"
#include "../ICanMessageSender.h"
#include "../../Helpers/DataBroker.h"
#include "../../../Config.h"
#include <inttypes.h>

/*
    VIN for head unit
*/
class MessageHandler_2B6 : public ICanMessageHandler
{
    Config* _config;

    public:
    MessageHandler_2B6(ICanMessageSender* object, DataBroker *dataBroker, Config* config) : ICanMessageHandler(object, dataBroker, 0x2B6, 8, 1000-100) {
        _config = config;
    };

    uint8_t SendMessage(unsigned long currentTime, bool forcedSend) override;
};
#endif
