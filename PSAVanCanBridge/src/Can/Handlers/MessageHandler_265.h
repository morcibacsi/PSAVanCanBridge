#pragma once

#ifndef _MessageHandler_265_h
    #define _MessageHandler_265_h

#include "ICanMessageHandler.h"
#include "../ICanMessageSender.h"
#include "../../../Config.h"
#include "../../Helpers/DataBroker.h"
#include <inttypes.h>

//2010 same as 2004 differences in bytes 2,3,4

/*
    Radio related data for the EMF
*/
class MessageHandler_265 : public ICanMessageHandler
{
    Config* _config;

    public:
    MessageHandler_265(ICanMessageSender* object, DataBroker *dataBroker, Config* config) : ICanMessageHandler(object, dataBroker, 0x265, 4, 500-30) { _config = config; };

    void SetData(uint8_t rds, uint8_t rds_ok, uint8_t ta, uint8_t ta_ok, uint8_t reg, uint8_t pty_not_selected);
    uint8_t SendMessage(unsigned long currentTime, bool forcedSend) override;
};
#endif
