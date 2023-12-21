#pragma once

#ifndef _MessageHandler_2A1_h
    #define _MessageHandler_2A1_h

#include "ICanMessageHandler.h"
#include "../ICanMessageSender.h"
#include "../../Helpers/DataBroker.h"
#include <inttypes.h>

// 2010 difference in byte 6-7

/*
    Trip computer data for EMF
*/
class MessageHandler_2A1 : public ICanMessageHandler
{
    public:
    MessageHandler_2A1(ICanMessageSender* object, DataBroker *dataBroker) : ICanMessageHandler(object, dataBroker, 0x2A1, 7, 1000-100) { };

    void SetData() override;
};
#endif
