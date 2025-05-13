#include "AEE2004ComfortBus.hpp"

#include "Handlers/AAS/MessageHandler_0E1.hpp"
#include "Handlers/BSI/MessageHandler_0B6.h"
#include "Handlers/BSI/MessageHandler_0E6.h"
#include "Handlers/BSI/MessageHandler_0E8.h"
#include "Handlers/BSI/MessageHandler_0F6.h"
#include "Handlers/BSI/MessageHandler_1A1.h"
#include "Handlers/BSI/MessageHandler_1A8.h"
#include "Handlers/BSI/MessageHandler_2A1.h"
#include "Handlers/BSI/MessageHandler_2B6.h"
#include "Handlers/BSI/MessageHandler_2E1.h"
#include "Handlers/BSI/MessageHandler_3A7.h"
#include "Handlers/BSI/MessageHandler_3B6.h"
#include "Handlers/BSI/MessageHandler_10B.h"
#include "Handlers/BSI/MessageHandler_21F.h"
#include "Handlers/BSI/MessageHandler_036.h"
#include "Handlers/BSI/MessageHandler_120.h"
#include "Handlers/BSI/MessageHandler_127.h"
#include "Handlers/BSI/MessageHandler_128.h"
#include "Handlers/BSI/MessageHandler_161.h"
#include "Handlers/BSI/MessageHandler_168.h"
#include "Handlers/CMB/MessageHandler_217.h"
#include "Handlers/BSI/MessageHandler_220.h"
#include "Handlers/BSI/MessageHandler_221.h"
#include "Handlers/BSI/MessageHandler_227.h"
#include "Handlers/BSI/MessageHandler_261.h"
#include "Handlers/BSI/MessageHandler_336.h"
#include "Handlers/BSI/MessageHandler_361.h"
#include "Handlers/CLV/MessageHandler_1E3.h"
#include "Handlers/EMF/MessageHandler_167.h"
#include "Handlers/VTH/MessageHandler_297.h"

#include <cstring>
#include <algorithm>
#include <esp_attr.h>

AEE2004ComfortBus::AEE2004ComfortBus(
        std::shared_ptr<CarState> carState,
        std::shared_ptr<ITransportLayer> transport,
        std::shared_ptr<MessageScheduler> scheduler
        )
{
    _carState = std::move(carState);
    _transportLayer = std::move(transport);
    _scheduler = std::move(scheduler);
    _messageHandlers = std::unordered_map<uint32_t, std::unique_ptr<IMessageHandler>>();
}

void AEE2004ComfortBus::RegisterMessageHandlers(std::function<void(ImmediateSignal)> immediateSignalCallback)
{
    _immediateSignalCallback = immediateSignalCallback;

    _messageHandlers.insert({0x0E1, std::make_unique<MessageHandler_0E1>()});

    _messageHandlers.insert({0x0B6, std::make_unique<MessageHandler_0B6>()});
    _messageHandlers.insert({0x0E6, std::make_unique<MessageHandler_0E6>()});
    _messageHandlers.insert({0x0E8, std::make_unique<MessageHandler_0E8>()});
    _messageHandlers.insert({0x0F6, std::make_unique<MessageHandler_0F6>()});
    _messageHandlers.insert({0x1A1, std::make_unique<MessageHandler_1A1>(_immediateSignalCallback)});
    _messageHandlers.insert({0x1A8, std::make_unique<MessageHandler_1A8>(_immediateSignalCallback)});
    _messageHandlers.insert({0x2A1, std::make_unique<MessageHandler_2A1>()});
    _messageHandlers.insert({0x2B6, std::make_unique<MessageHandler_2B6>()});
    _messageHandlers.insert({0x2E1, std::make_unique<MessageHandler_2E1>()});
    _messageHandlers.insert({0x3A7, std::make_unique<MessageHandler_3A7>()});
    _messageHandlers.insert({0x3B6, std::make_unique<MessageHandler_3B6>()});
    _messageHandlers.insert({0x10B, std::make_unique<MessageHandler_10B>()});
    _messageHandlers.insert({0x21F, std::make_unique<MessageHandler_21F>(_immediateSignalCallback)});
    _messageHandlers.insert({0x036, std::make_unique<MessageHandler_036>()});
    _messageHandlers.insert({0x120, std::make_unique<MessageHandler_120>()});
    _messageHandlers.insert({0x127, std::make_unique<MessageHandler_127>()});
    _messageHandlers.insert({0x128, std::make_unique<MessageHandler_128>(_immediateSignalCallback)});
    _messageHandlers.insert({0x161, std::make_unique<MessageHandler_161>()});
    _messageHandlers.insert({0x168, std::make_unique<MessageHandler_168>(_immediateSignalCallback)});
    _messageHandlers.insert({0x220, std::make_unique<MessageHandler_220>(_immediateSignalCallback)});
    _messageHandlers.insert({0x221, std::make_unique<MessageHandler_221>(_immediateSignalCallback)});
    _messageHandlers.insert({0x227, std::make_unique<MessageHandler_227>()});
    _messageHandlers.insert({0x261, std::make_unique<MessageHandler_261>()});
    _messageHandlers.insert({0x336, std::make_unique<MessageHandler_336>()});
    _messageHandlers.insert({0x361, std::make_unique<MessageHandler_361>()});

    _messageHandlers.insert({0x1E3, std::make_unique<MessageHandler_1E3>()});

    _messageHandlers.insert({0x217, std::make_unique<MessageHandler_217>(_immediateSignalCallback)});

    //_messageHandlers.insert({0x167, std::make_unique<MessageHandler_167>()});

    //_messageHandlers.insert({0x297, std::make_unique<MessageHandler_297>()});
}

bool IRAM_ATTR AEE2004ComfortBus::ReceiveMessage(BusMessage& message)
{
    message.protocol = ProtocolType::AEE2004;

    bool result = _transportLayer->ReceiveMessage(message);
/*
    if (result)
    {
        switch (message.id)
        {
            case 0x036:
            case 0x0F6:
            case 0x128:
            case 0x21F:
            case 0x221:
                message.priority = 1;
                break;

            default:
                message.priority = 0;
                break;
        }
    }
*/
    return result;
}

void IRAM_ATTR AEE2004ComfortBus::ParseMessage(const BusMessage& message)
{
    //search for the message id in the map and if exists call the parse function
    auto it = _messageHandlers.find(message.id);

    if (it == _messageHandlers.end())
    {
        return;
    }

    it->second->Parse(_carState, message);
}

void AEE2004ComfortBus::GenerateMessages(MessageDirection direction)
{
    // Generate messages based on the car state and send them via the transport layer.

    if (direction == MessageDirection::Source)
    {
        //GenerateMessagesForSource();
        return;
    }

    if (direction == MessageDirection::Destination)
    {
        for (auto& [id, generator] : _messageHandlers)
        {
            BusMessage message = generator->Generate(_carState);
            _scheduler->AddOrUpdateMessage(message, _carState->CurrenTime);
        }
        return;
    }

///*
//*/
    //
    /*
    CanIgnitionByte5Struct ignitionField;
    if (_carState->Ignition)
    {
        ignitionField.data.ignition_mode = CAN_IGNITION_MODE_NORMAL;
    }
    else
    {
        ignitionField.data.ignition_mode = CAN_IGNITION_MODE_STANDBY;
    }

    //uint8_t data_036[] = { 0x0E, 0x00, 0x00, 0x21, 0x21, 0x80, 0x00, 0xA0 };
    //_transportLayer->SendMessage({0x036, {0x0E, 0x00, 0x00, 0x21, 0x21, 0x80, 0x00, 0xA0}, {}, false});

    //uint8_t data_0f6[] = { 0x8E, 0x71, 0x21, 0x8D, 0xF3, 0x65, 0x65, 0x50 };
    uint8_t temperature = _carState->ExternalTemperature;
    //_transportLayer->SendMessage({0x0F6, {0x8E, 0x71, 0x21, 0x8D, 0xF3, 0x65, temperature, 0x50}, {}, false});

    //_transportLayer->SendMessage({0x036, {0x0E, 0x00, 0x00, 0x21, 0x21, 0x80, 0x00, 0xA0}, {}, false});
    //_transportLayer->SendMessage({0x036, {0x0E, 0x00, 0x00, 0x21, ignitionField.asByte, 0x80, 0x00, 0xA0}, {}, false});

    BusMessage msg_0f6 = {0x0F6, {0x8E, 0x71, 0x21, 0x8D, 0xF3, 0x65, temperature, 0x50}, {}, false, AEE2004, 100};
    _scheduler->AddOrUpdateMessage(msg_0f6, _carState->CurrenTime, _carState->CurrenTime); // Periodicity: 100ms

    BusMessage msg_036 = {0x036, {0x0E, 0x00, 0x00, 0x21, ignitionField.asByte, 0x80, 0x00, 0xA0}, {}, false, AEE2004, 100};
    _scheduler->AddOrUpdateMessage(msg_036, _carState->CurrenTime, _carState->CurrenTime); // Periodicity: 100ms
    //*/
}

void AEE2004ComfortBus::HandleFeedbackSignal(FeedbackSignal signal)
{
    // React to signals and send immediate messages via the transport layer.
    switch (signal)
    {
        case FeedbackSignal::IgnitionChanged:
        {
            /*
            if (_carState->IsReverseEngaged)
            {
                auto aasHandler = _messageHandlersForSource.find(0xA68);
                if (aasHandler != _messageHandlersForSource.end())
                {
                    auto message = aasHandler->second->Generate(_carState);
                    _transportLayer->SendMessage(message, true);
                }
            }
            */

            //_canPopupHandler->SetIgnition(_carState->CurrenTime, _carState->Ignition);
            break;
        }
        default:
            break;
    }


    if (signal == FeedbackSignal::GetTripComputerData)
    {
        BusMessage immediateMsg;
        immediateMsg.id = 0x400;
        immediateMsg.data[0] = 0x01;
        immediateMsg.dataLength = 1;
        //_transportLayer->SendMessage(immediateMsg);
    }
}

void AEE2004ComfortBus::UpdateMessages(uint64_t currentTime)
{
    // Let the scheduler manage periodic sending.
    _scheduler->Update(currentTime, *_transportLayer);
}

void AEE2004ComfortBus::ProcessImmediateSignal(ImmediateSignal signal)
{
    switch (signal)
    {
        case ImmediateSignal::RadioRemote:
        {
            SendImmediateMessage(0x21F);
            break;
        }
        case ImmediateSignal::TripButtonPressed:
        {
            SendImmediateMessage(0x221);
            break;
        }
        case ImmediateSignal::ReverseChanged:
        {
            SendImmediateMessage(0x0F6);
            SendImmediateMessage(0x036);
            break;
        }
        case ImmediateSignal::CruiseControl:
        {
            SendImmediateMessage(0x1A8);
            SendImmediateMessage(0x228);
            SendImmediateMessage(0x128);
            SendImmediateMessage(0x168);
            break;
        }
        case ImmediateSignal::SpeedAndRpm:
        {
            SendImmediateMessage(0x0B6);
            SendImmediateMessage(0x297);
            break;
        }
        default:
        break;
    }
}

void AEE2004ComfortBus::SendImmediateMessage(uint32_t id)
{
    //printf("AEEE2004 SendImmediateMessage: %X\n", (unsigned int)id);

    std::unordered_map<uint32_t, std::unique_ptr<IMessageHandler>>::iterator handler;
    handler = _messageHandlers.find(id);

    if (handler == _messageHandlers.end())
    {
        return;
    }
    auto generator = handler->second.get();
    BusMessage message = generator->Generate(_carState);
    _scheduler->AddOrUpdateMessage(message, _carState->CurrenTime);

    _scheduler->SendImmedateMessage(id, _carState->CurrenTime, *_transportLayer);
}

bool IRAM_ATTR AEE2004ComfortBus::CanParseMessage(const BusMessage& message)
{
    return _messageHandlers.find(message.id) != _messageHandlers.end();
}