#pragma once

#ifndef _MessageHandler_120_h
    #define _MessageHandler_120_h

#include "ICanMessageHandler.h"
#include "../ICanMessageSender.h"
#include "../../Helpers/DataBroker.h"
#include <inttypes.h>

/*
    Warning/Alert log
*/
class MessageHandler_120 : public ICanMessageHandler
{
    uint8_t _preHeatingStatus;

    public:
    MessageHandler_120(ICanMessageSender* object, DataBroker *dataBroker) : ICanMessageHandler(object, dataBroker, 0x120, 8, 1000-100) {
        _data[0] = 0xFF;
     };

    void SetData() override;
};
#endif
