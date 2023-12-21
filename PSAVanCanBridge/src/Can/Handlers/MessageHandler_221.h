#pragma once

#ifndef _MessageHandler_221_h
    #define _MessageHandler_221_h

#include "ICanMessageHandler.h"
#include "../ICanMessageSender.h"
#include "../../Helpers/DataBroker.h"
#include "../../../Config.h"
#include <inttypes.h>

//2010 slight difference in bytes 1

/*
    Trip computer data
*/
class MessageHandler_221 : public ICanMessageHandler
{
    Config* _config;

    public:
    MessageHandler_221(ICanMessageSender* object, DataBroker *dataBroker, Config* config) : ICanMessageHandler(object, dataBroker, 0x221, 7, 1000)
    {
        _config = config;
    };

    void TripButtonPress();
    void SetData() override;
};
#endif
