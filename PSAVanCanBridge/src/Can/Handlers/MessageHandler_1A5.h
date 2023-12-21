#pragma once

#ifndef _MessageHandler_1A5_h
    #define _MessageHandler_1A5_h

#include "ICanMessageHandler.h"
#include "../ICanMessageSender.h"
#include "../../../Config.h"
#include "../../Helpers/DataBroker.h"
#include <inttypes.h>

/*
    Radio volume data for EMF
*/
class MessageHandler_1A5 : public ICanMessageHandler
{
    Config* _config;

    public:
    MessageHandler_1A5(ICanMessageSender* object, DataBroker *dataBroker, Config* config) : ICanMessageHandler(object, dataBroker, 0x1A5, 1, 500-30) { _config = config; };

    void SetData(uint8_t origin, uint8_t volume);
    uint8_t SendMessage(unsigned long currentTime, bool forcedSend) override;
};
#endif
