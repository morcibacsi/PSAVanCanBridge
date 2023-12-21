#pragma once

#ifndef _MessageHandler_261_h
    #define _MessageHandler_261_h

#include "ICanMessageHandler.h"
#include "../ICanMessageSender.h"
#include "../../Helpers/DataBroker.h"
#include <inttypes.h>

//2010 slight difference in bytes 6, 7

/*
    Trip computer data
*/
class MessageHandler_261 : public ICanMessageHandler
{
    public:
    MessageHandler_261(ICanMessageSender* object, DataBroker *dataBroker) : ICanMessageHandler(object, dataBroker, 0x261, 7, 1000-100) { };

    void SetData() override;
};
#endif
