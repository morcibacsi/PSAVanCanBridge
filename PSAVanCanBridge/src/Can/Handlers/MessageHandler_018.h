#pragma once

#ifndef _MessageHandler_018_h
    #define _MessageHandler_018_h

#include "ICanMessageHandler.h"
#include "../ICanMessageSender.h"
#include "../../Helpers/DataBroker.h"
#include <inttypes.h>

//2010 slight difference in bytes 3,4

/*
    Crash info for telematics
*/
class MessageHandler_018 : public ICanMessageHandler
{
    public:
    MessageHandler_018(ICanMessageSender* object, DataBroker *dataBroker) : ICanMessageHandler(object, dataBroker, 0x018, 5, 200-30) { };
};
#endif
