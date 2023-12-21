#pragma once

#ifndef _MessageHandler_161_h
    #define _MessageHandler_161_h

#include "ICanMessageHandler.h"
#include "../ICanMessageSender.h"
#include "../../Helpers/DataBroker.h"
#include <inttypes.h>

//2010 almost the same as 2004 has some extended information

/*
    Fuel level, oil level, oil temperature
*/
class MessageHandler_161  : public ICanMessageHandler
{
    public:
    MessageHandler_161(ICanMessageSender* object, DataBroker *dataBroker) : ICanMessageHandler(object, dataBroker, 0x161, 7, 500) { };

    void SetData() override;
};
#endif
