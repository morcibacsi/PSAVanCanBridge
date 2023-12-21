#pragma once

#ifndef _MessageHandler_220_h
    #define _MessageHandler_220_h

#include "ICanMessageHandler.h"
#include "../ICanMessageSender.h"
#include "../../Helpers/DataBroker.h"
#include <inttypes.h>

//2010 does not have this message

/*
    Door status
*/
class MessageHandler_220 : public ICanMessageHandler
{
    uint8_t _preHeatingStatus;

    public:
    MessageHandler_220(ICanMessageSender* object, DataBroker *dataBroker) : ICanMessageHandler(object, dataBroker, 0x220, 2, 500) { };

    void SetData() override;
};
#endif
