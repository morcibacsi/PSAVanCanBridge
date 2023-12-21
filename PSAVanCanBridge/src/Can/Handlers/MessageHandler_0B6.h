#pragma once

#ifndef _MessageHandler_0B6_h
    #define _MessageHandler_0B6_h

#include "ICanMessageHandler.h"
#include "../ICanMessageSender.h"
#include "../../Helpers/DataBroker.h"
#include <inttypes.h>

/*
    Speed, rpm, consumption on CMB
*/
class MessageHandler_0B6 : public ICanMessageHandler
{
    public:
    MessageHandler_0B6(ICanMessageSender* object, DataBroker *dataBroker) : ICanMessageHandler(object, dataBroker, 0x0B6, 8, 50) { };

    void SetData() override;
};
#endif
