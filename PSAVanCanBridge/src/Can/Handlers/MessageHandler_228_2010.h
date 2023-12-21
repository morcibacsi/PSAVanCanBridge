#pragma once

#ifndef _MessageHandler_228_2010_h
    #define _MessageHandler_228_2010_h

#include "ICanMessageHandler.h"
#include "../ICanMessageSender.h"
#include "../../Helpers/DataBroker.h"
#include "../../../Config.h"
#include <inttypes.h>

// 2004: 0x1A8

/*
    Cruise control related data for CMB, MATT
*/
class MessageHandler_228_2010 : public ICanMessageHandler
{
    Config* _config;

    public:
    MessageHandler_228_2010(ICanMessageSender* object, DataBroker *dataBroker, Config* config) : ICanMessageHandler(object, dataBroker, 0x228, 8, 1000) {
        _config = config;
    };

    void SetData() override;
    uint8_t SendMessage(unsigned long currentTime, bool forcedSend) override;
};
#endif
