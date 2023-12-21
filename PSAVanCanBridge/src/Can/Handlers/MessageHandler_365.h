#pragma once

#ifndef _MessageHandler_365_h
    #define _MessageHandler_365_h

#include "ICanMessageHandler.h"
#include "../ICanMessageSender.h"
#include "../../../Config.h"
#include "../../Helpers/DataBroker.h"
#include <stdint.h>

//2010 doesn't exists most probably there is a new universal frame for this type of information

/*
    CD related data for the EMF
*/
class MessageHandler_365 : public ICanMessageHandler
{
    Config* _config;

    public:
    MessageHandler_365(ICanMessageSender* object, DataBroker *dataBroker, Config* config) : ICanMessageHandler(object, dataBroker, 0x365, 5, 500-30) { _config = config; };

    void SetData(uint8_t number_of_tracks, uint8_t total_minutes, uint8_t total_seconds);
    uint8_t SendMessage(unsigned long currentTime, bool forcedSend) override;
};
#endif
