#pragma once

#ifndef _MessageHandler_126_h
    #define _MessageHandler_126_h

#include "ICanMessageHandler.h"
#include "../ICanMessageSender.h"
#include "../../Helpers/DataBroker.h"
#include <inttypes.h>

//2010 slight differences in byte 1 and extended with additional information

/*
    Reverse signal for parking radar
*/
class MessageHandler_126 : public ICanMessageHandler
{
    uint8_t _preHeatingStatus;

    public:
    MessageHandler_126(ICanMessageSender* object, DataBroker *dataBroker) : ICanMessageHandler(object, dataBroker, 0x126, 3, 100) { };

    void SetData() override;
};
#endif
