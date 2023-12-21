#pragma once

#ifndef _MessageHandler_10B_h
    #define _MessageHandler_10B_h

#include "ICanMessageHandler.h"
#include "../ICanMessageSender.h"
#include "../../Helpers/DataBroker.h"
#include <inttypes.h>

//2010 7 bytes long

/*
    Steering wheel angle data
*/
class MessageHandler_10B : public ICanMessageHandler
{
    public:
    MessageHandler_10B(ICanMessageSender* object, DataBroker *dataBroker) : ICanMessageHandler(object, dataBroker, 0x10B, 6, 100-20) { };

    void SetData() override;
};
#endif
