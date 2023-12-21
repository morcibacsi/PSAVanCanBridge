#pragma once

#ifndef _MessageHandler_3E7_2010_h
    #define _MessageHandler_3E7_2010_h

#include "ICanMessageHandler.h"
#include "../ICanMessageSender.h"
#include "../../Helpers/DataBroker.h"
#include "../../../Config.h"
#include <inttypes.h>

//2004 completely different look for 0x3A7

/*
    Maintenance data on CMB
*/
class MessageHandler_3E7_2010 : public ICanMessageHandler
{
    Config* _config;

    public:
    MessageHandler_3E7_2010(ICanMessageSender* object, DataBroker *dataBroker, Config* config) : ICanMessageHandler(object, dataBroker, 0x3E7, 5, 500)
    {
        _config = config;
    };

    void SetData() override;
    uint8_t SendMessage(unsigned long currentTime, bool forcedSend) override;
};
#endif
