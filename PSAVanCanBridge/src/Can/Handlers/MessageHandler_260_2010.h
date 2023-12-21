#pragma once

#ifndef _MessageHandler_260_2010_h
    #define _MessageHandler_260_2010_h

#include "ICanMessageHandler.h"
#include "../ICanMessageSender.h"
#include "../../Helpers/DataBroker.h"
#include "../../../Config.h"
#include <inttypes.h>

/*
    Vehicle settings
*/
class MessageHandler_260_2010 : public ICanMessageHandler
{
    Config* _config;

    public:
    MessageHandler_260_2010(ICanMessageSender* object, DataBroker *dataBroker, Config* config) : ICanMessageHandler(object, dataBroker, 0x260, 8, 500) {
        _config = config;
    };

    void SetData() override;
    uint8_t SendMessage(unsigned long currentTime, bool forcedSend) override;
};
#endif
