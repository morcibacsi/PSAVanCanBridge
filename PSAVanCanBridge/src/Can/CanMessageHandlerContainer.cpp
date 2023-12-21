#include "CanMessageHandlerContainer.h"
#include "Handlers/MessageHandler_036.h"
#include "Handlers/MessageHandler_0F6.h"
#include "Handlers/MessageHandler_128.h"
#include "Handlers/MessageHandler_161.h"
#include "Handlers/MessageHandler_1A8.h"
#include "Handlers/MessageHandler_3A7.h"
#include "Handlers/MessageHandler_3E7_2010.h"
#include "Handlers/MessageHandler_21F.h"
#include "Handlers/MessageHandler_0E6.h"
#include "Handlers/MessageHandler_0E1.h"
#include "Handlers/MessageHandler_168.h"
#include "Handlers/MessageHandler_0B6.h"
#include "Handlers/MessageHandler_221.h"
#include "Handlers/MessageHandler_2A1.h"
#include "Handlers/MessageHandler_261.h"
#include "Handlers/MessageHandler_336.h"
#include "Handlers/MessageHandler_3B6.h"
#include "Handlers/MessageHandler_2B6.h"
#include "Handlers/MessageHandler_10B.h"
#include "Handlers/MessageHandler_1A1.h"
#include "Handlers/MessageHandler_1E3.h"
#include "Handlers/MessageHandler_350_2010.h"
#include "Handlers/MessageHandler_126.h"
#include "Handlers/MessageHandler_018.h"
#include "Handlers/MessageHandler_228_2010.h"
#include "Handlers/MessageHandler_2E1.h"
#include "Handlers/MessageHandler_120.h"
#include "Handlers/MessageHandler_220.h"
#include "Handlers/MessageHandler_260_2010.h"
#include "Handlers/MessageHandler_1D0.h"
// Tuner
#include "Handlers/MessageHandler_165.h"
#include "Handlers/MessageHandler_1A5.h"
#include "Handlers/MessageHandler_225.h"
#include "Handlers/MessageHandler_265.h"
#include "Handlers/MessageHandler_1E5.h"
#include "Handlers/MessageHandler_2A5.h"
// CD
#include "Handlers/MessageHandler_365.h"
#include "Handlers/MessageHandler_325.h"
#include "Handlers/MessageHandler_3A5.h"
// Radio button
#include "Handlers/MessageHandler_3E5.h"
// EMF
#include "Handlers/MessageHandler_167.h"
// MATT
#include "Handlers/MessageHandler_297.h"

CanMessageHandlerContainer::CanMessageHandlerContainer(ICanMessageSender *canInterface, Config *config, DataBroker *dataBroker)
{
    _handlers.push_back(new MessageHandler_036(canInterface, dataBroker, config));
    _handlers.push_back(new MessageHandler_0F6(canInterface, dataBroker));
    _handlers.push_back(new MessageHandler_128(canInterface, dataBroker, config));
    _handlers.push_back(new MessageHandler_161(canInterface, dataBroker));
    _handlers.push_back(new MessageHandler_1A8(canInterface, dataBroker, config));
    _handlers.push_back(new MessageHandler_3A7(canInterface, dataBroker, config));
    _handlers.push_back(new MessageHandler_3E7_2010(canInterface, dataBroker, config));
    _handlers.push_back(new MessageHandler_21F(canInterface, dataBroker, config));
    _handlers.push_back(new MessageHandler_0E6(canInterface, dataBroker, config));
    _handlers.push_back(new MessageHandler_0E1(canInterface, dataBroker, config));
    _handlers.push_back(new MessageHandler_168(canInterface, dataBroker, config));
    _handlers.push_back(new MessageHandler_0B6(canInterface, dataBroker));
    _handlers.push_back(new MessageHandler_221(canInterface, dataBroker, config));
    _handlers.push_back(new MessageHandler_2A1(canInterface, dataBroker));
    _handlers.push_back(new MessageHandler_261(canInterface, dataBroker));
    _handlers.push_back(new MessageHandler_336(canInterface, dataBroker, config));
    _handlers.push_back(new MessageHandler_3B6(canInterface, dataBroker, config));
    _handlers.push_back(new MessageHandler_2B6(canInterface, dataBroker, config));
    _handlers.push_back(new MessageHandler_10B(canInterface, dataBroker));
    _handlers.push_back(new MessageHandler_1A1(canInterface, dataBroker));
    _handlers.push_back(new MessageHandler_1E3(canInterface, dataBroker, config));
    _handlers.push_back(new MessageHandler_350_2010(canInterface, dataBroker, config));
    _handlers.push_back(new MessageHandler_126(canInterface, dataBroker));
    _handlers.push_back(new MessageHandler_018(canInterface, dataBroker));
    _handlers.push_back(new MessageHandler_228_2010(canInterface, dataBroker, config));
    _handlers.push_back(new MessageHandler_2E1(canInterface, dataBroker));
    _handlers.push_back(new MessageHandler_120(canInterface, dataBroker));
    _handlers.push_back(new MessageHandler_220(canInterface, dataBroker));
    _handlers.push_back(new MessageHandler_260_2010(canInterface, dataBroker, config));
    _handlers.push_back(new MessageHandler_1D0(canInterface, dataBroker, config));

    // Tuner
    _handlers.push_back(new MessageHandler_165(canInterface, dataBroker, config));
    _handlers.push_back(new MessageHandler_1A5(canInterface, dataBroker, config));
    _handlers.push_back(new MessageHandler_225(canInterface, dataBroker, config));
    _handlers.push_back(new MessageHandler_265(canInterface, dataBroker, config));
    _handlers.push_back(new MessageHandler_1E5(canInterface, dataBroker, config));
    _handlers.push_back(new MessageHandler_2A5(canInterface, dataBroker, config));

    // CD
    _handlers.push_back(new MessageHandler_365(canInterface, dataBroker, config));
    _handlers.push_back(new MessageHandler_325(canInterface, dataBroker, config));
    _handlers.push_back(new MessageHandler_3A5(canInterface, dataBroker, config));

    _handlers.push_back(new MessageHandler_3E5(canInterface, dataBroker, config));
    _handlers.push_back(new MessageHandler_167(canInterface, dataBroker, config));
    _handlers.push_back(new MessageHandler_297(canInterface, dataBroker));
}

ICanMessageHandler* CanMessageHandlerContainer::GetHandler(uint16_t canId)
{
    for (ICanMessageHandler* handler : _handlers)
    {
        if (handler->GetCanId() == canId)
        {
            return handler;
        }
    }

    return nullptr;
}

void CanMessageHandlerContainer::SendDueMessages(unsigned long currentTime)
{
    for (ICanMessageHandler* handler : _handlers)
    {
        handler->SendMessage(currentTime, false);
    }
}

void CanMessageHandlerContainer::SendMessageForced(uint16_t canId, unsigned long currentTime)
{
    ICanMessageHandler* handler = GetHandler(canId);

    if (handler != nullptr)
    {
        handler->SendMessage(currentTime, true);
    }
}

void CanMessageHandlerContainer::SetData(uint16_t canId)
{
    ICanMessageHandler* handler = GetHandler(canId);

    if (handler != nullptr)
    {
        handler->SetData();
    }
}