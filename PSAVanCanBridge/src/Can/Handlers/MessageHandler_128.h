#pragma once

#ifndef _MessageHandler_128_h
    #define _MessageHandler_128_h

#include "ICanMessageHandler.h"
#include "../ICanMessageSender.h"
#include "../../Helpers/DataBroker.h"
#include "../../../Config.h"
#include <inttypes.h>

//2010 same info but completely different structure

/*
    Lights on CMB (fuel level, turn signals, headlight, etc.)
*/
class MessageHandler_128 : public ICanMessageHandler
{
    Config* _config;
    uint8_t _preHeatingStatus;

    void SetData2004();
    void SetData2010();

    public:
    MessageHandler_128(ICanMessageSender* object, DataBroker *dataBroker, Config* config) : ICanMessageHandler(object, dataBroker, 0x128, 8, 200)
    {
        _config = config;
    };

    void SetData() override;
};
#endif
