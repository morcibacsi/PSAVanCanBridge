#pragma once

#ifndef _MessageHandler_2E1_h
    #define _MessageHandler_2E1_h

#include "ICanMessageHandler.h"
#include "../ICanMessageSender.h"
#include "../../Helpers/DataBroker.h"
#include <inttypes.h>

//2010 different length, extended with additional information

/*
    Status of functions
*/
class MessageHandler_2E1 : public ICanMessageHandler
{
    public:
    MessageHandler_2E1(ICanMessageSender* object, DataBroker *dataBroker) : ICanMessageHandler(object, dataBroker, 0x2E1, 5, 1000) { };

    void SetData() override;
};
#endif
