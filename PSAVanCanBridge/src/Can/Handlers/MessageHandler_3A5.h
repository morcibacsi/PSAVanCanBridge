#pragma once

#ifndef _MessageHandler_3A5_h
    #define _MessageHandler_3A5_h

#include "ICanMessageHandler.h"
#include "../ICanMessageSender.h"
#include "../../../Config.h"
#include "../../Helpers/DataBroker.h"
#include <inttypes.h>

//2010 completely different

/*
    CD track data for EMF
*/
class MessageHandler_3A5 : public ICanMessageHandler
{
    Config* _config;

    public:
    MessageHandler_3A5(ICanMessageSender* object, DataBroker *dataBroker, Config* config) : ICanMessageHandler(object, dataBroker, 0x3A5, 6, 500-30) { _config = config; };

    void SetData(uint8_t number_of_current_track, uint8_t total_track_minutes, uint8_t total_track_seconds, uint8_t elapsed_track_minutes, uint8_t elapsed_track_seconds);
    uint8_t SendMessage(unsigned long currentTime, bool forcedSend) override;
};
#endif
