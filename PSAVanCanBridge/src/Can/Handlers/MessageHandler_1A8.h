#pragma once

#ifndef _MessageHandler_1A8_h
    #define _MessageHandler_1A8_h

#include "ICanMessageHandler.h"
#include "../ICanMessageSender.h"
#include "../../Helpers/DataBroker.h"
#include "../../../Config.h"
#include <inttypes.h>

//2010 completely different look for 0x228

/*
    Cruise control and trip data for CMB
*/
class MessageHandler_1A8 : public ICanMessageHandler
{
    Config* _config;

    public:
    MessageHandler_1A8(ICanMessageSender* object, DataBroker *dataBroker, Config* config) : ICanMessageHandler(object, dataBroker, 0x1A8, 8, 200) {
        _config = config;
    };

    void SetData() override;
    //void SendMessage(unsigned long currentTime, bool forcedSend) override;
};
#endif
