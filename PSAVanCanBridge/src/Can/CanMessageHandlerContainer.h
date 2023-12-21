#pragma once

#ifndef _CanMessageHandlerContainer_h
    #define _CanMessageHandlerContainer_h


#include "ICanMessageSender.h"
#include "Handlers/ICanMessageHandler.h"
#include "../../Config.h"
#include "../Helpers/DataBroker.h"
#include <inttypes.h>
#include <list>

class CanMessageHandlerContainer
{
    std::list<ICanMessageHandler*> _handlers;

    public:
    CanMessageHandlerContainer(ICanMessageSender *canInterface, Config *config, DataBroker *dataBroker);

    ICanMessageHandler* GetHandler(uint16_t canId);
    void SendDueMessages(unsigned long currentTime);
    void SendMessageForced(uint16_t canId, unsigned long currentTime);
    void SetData(uint16_t canId);
};
#endif
