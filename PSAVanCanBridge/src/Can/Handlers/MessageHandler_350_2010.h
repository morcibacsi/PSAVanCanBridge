#pragma once

#ifndef _MessageHandler_350_2010_h
    #define _MessageHandler_350_2010_h

#include "ICanMessageHandler.h"
#include "../ICanMessageSender.h"
#include "../../../Config.h"
#include "../../Helpers/DataBroker.h"
#include <inttypes.h>

//2004 does not exist, look for 0x1E3

/*
    A/C data for display
*/
class MessageHandler_350_2010 : public ICanMessageHandler
{
    Config* _config;

    public:
    MessageHandler_350_2010(ICanMessageSender* object, DataBroker *dataBroker, Config* config) : ICanMessageHandler(object, dataBroker, 0x350, 8, 500)
    {
        _config = config;
        _data[5] = 0x1;
    };

    void SetData() override;
    uint8_t SendMessage(unsigned long currentTime, bool forcedSend) override;
};
#endif
