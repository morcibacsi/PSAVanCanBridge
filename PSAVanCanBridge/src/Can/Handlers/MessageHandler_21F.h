#pragma once

#ifndef _MessageHandler_21F_h
    #define _MessageHandler_21F_h

#include "ICanMessageHandler.h"
#include "../ICanMessageSender.h"
#include "../../../Config.h"
#include "../../Helpers/DataBroker.h"
#include <inttypes.h>

//2010 slight difference in byte 3, but compatible with 2004

/*
    Steering remote data for radio
*/
class MessageHandler_21F : public ICanMessageHandler
{
    Config* _config;

    public:
    MessageHandler_21F(ICanMessageSender* object, DataBroker *dataBroker, Config* config) : ICanMessageHandler(object, dataBroker, 0x21F, 3, 100)
    {
        _config = config;
    };

    void SetData() override;
    uint8_t SendMessage(unsigned long currentTime, bool forcedSend) override;
};
#endif
