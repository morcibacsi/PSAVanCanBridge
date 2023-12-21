#pragma once

#ifndef _MessageHandler_225_h
    #define _MessageHandler_225_h

#include "ICanMessageHandler.h"
#include "../ICanMessageSender.h"
#include "../../../Config.h"
#include "../../Helpers/DataBroker.h"
#include <inttypes.h>

//2010 same info, different structure

/*
    Radio data for EMF
*/
class MessageHandler_225 : public ICanMessageHandler
{
    Config* _config;

    public:
    MessageHandler_225(ICanMessageSender* object, DataBroker *dataBroker, Config* config) : ICanMessageHandler(object, dataBroker, 0x225, 5, 500-30) { _config = config; };

    void SetData(uint8_t frequency_scan_status, uint8_t tuner_sensitivity, uint8_t memo_num, uint8_t band, uint8_t freq1, uint8_t freq2);
    uint8_t SendMessage(unsigned long currentTime, bool forcedSend) override;
};
#endif
