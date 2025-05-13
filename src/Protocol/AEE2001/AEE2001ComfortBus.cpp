#include <string.h>
#include <esp_attr.h>

#include "AEE2001ComfortBus.hpp"

#include "Handlers/BSI/MessageHandler_4FC.h"
#include "Handlers/BSI/MessageHandler_4DC.h"
#include "Handlers/BSI/MessageHandler_8A4.h"
#include "Handlers/BSI/MessageHandler_8C4.h"
#include "Handlers/BSI/MessageHandler_9C4.h"
#include "Handlers/BSI/MessageHandler_524.h"
#include "Handlers/BSI/MessageHandler_564.h"
#include "Handlers/BSI/MessageHandler_744.h"
#include "Handlers/BSI/MessageHandler_824.h"
#include "Handlers/BSI/MessageHandler_A68.h"
#include "Handlers/BSI/MessageHandler_AE8.h"

#include "Handlers/CLIM/MessageHandler_464.h"

#include "Handlers/EMF/MessageHandler_5E4.h"

AEE2001ComfortBus::AEE2001ComfortBus(
    std::shared_ptr<CarState> carState,
    std::shared_ptr<ITransportLayer> transport,
    std::shared_ptr<MessageScheduler> scheduler
    )
{
    _transportLayer = std::move(transport);
    _schedulerForSourceNetwork = std::move(scheduler);
    _carState = std::move(carState);
    _messageHandlers = std::unordered_map<uint32_t, std::shared_ptr<IMessageHandler>>();
    _messageHandlersForSource = std::unordered_map<uint32_t, std::shared_ptr<IMessageHandler>>();

    _canPopupHandler = std::make_shared<CanDisplayPopupHandler3>(_carState);
}

void AEE2001ComfortBus::RegisterMessageHandlers(std::function<void(ImmediateSignal)> immediateSignalCallback)
{
    _immediateSignalCallback = immediateSignalCallback;

    if (_carState->EMULATE_DISPLAY_ON_SOURCE)
    {
        _messageHandlersForSource.insert({0x5E4, std::make_unique<MessageHandler_5E4>()});
    }

    if (_carState->PARKING_AID_TYPE == 0x01)
    {
        auto _aasHandler = std::make_shared<MessageHandler_A68>([this](FeedbackSignal signal) { this->HandleFeedbackSignal(signal); });

        _messageHandlersForSource.insert({0xA68, _aasHandler});

        _messageHandlers.insert({0xA68, _aasHandler});
        _messageHandlers.insert({0xAE8, std::make_shared<MessageHandler_AE8>()});
    }

    _messageHandlers.insert({0x4DC, std::make_shared<MessageHandler_4DC>()});
    _messageHandlers.insert({0x4FC, std::make_shared<MessageHandler_4FC>(_immediateSignalCallback)});
    _messageHandlers.insert({0x8A4, std::make_shared<MessageHandler_8A4>(_canPopupHandler, _immediateSignalCallback, [this](FeedbackSignal signal) { this->HandleFeedbackSignal(signal); })});
    _messageHandlers.insert({0x8C4, std::make_shared<MessageHandler_8C4>([this](FeedbackSignal signal) { this->HandleFeedbackSignal(signal); })});
    _messageHandlers.insert({0x9C4, std::make_shared<MessageHandler_9C4>(_immediateSignalCallback)});
    _messageHandlers.insert({0x524, std::make_shared<MessageHandler_524>(_canPopupHandler)});
    _messageHandlers.insert({0x564, std::make_shared<MessageHandler_564>(_canPopupHandler, _immediateSignalCallback)});
    _messageHandlers.insert({0x744, std::make_shared<MessageHandler_744>()});
    _messageHandlers.insert({0x824, std::make_shared<MessageHandler_824>(_immediateSignalCallback)});

    _messageHandlers.insert({0x464, std::make_shared<MessageHandler_464>()});
}

bool AEE2001ComfortBus::ReceiveMessage(BusMessage& message)
{
    message.protocol = ProtocolType::AEE2001;
    return _transportLayer->ReceiveMessage(message);
}

void AEE2001ComfortBus::ParseMessage(const BusMessage& message)
{
    //printf("AEE2001ComfortBus::ParseMessage: %X\n", (unsigned int)message.id);
    //search for the message id in the map and if exists call the parse function
    auto it = _messageHandlers.find(message.id);

    if (it == _messageHandlers.end())
    {
        return;
    }
    //printf("AEE2001ComfortBus::ParseMessage: %X\n", (unsigned int)message.id);
    if (it->second == nullptr)
    {
        //printf("AEE2001ComfortBus::ParseMessage: it->second == nullptr\n");
        return;
    }

    it->second->Parse(_carState, message);
}

void AEE2001ComfortBus::GenerateMessages(MessageDirection direction)
{
    if (direction == MessageDirection::Source)
    {
        GenerateMessagesForSource();
        _canPopupHandler->Process(_carState->CurrenTime);
        return;
    }

    if (direction == MessageDirection::Destination)
    {
        return;
    }
}

void AEE2001ComfortBus::GenerateMessagesForSource()
{
    for (auto& [id, generator] : _messageHandlersForSource)
    {
        BusMessage message = generator->Generate(_carState);
        _schedulerForSourceNetwork->AddOrUpdateMessage(message, _carState->CurrenTime);
    }
}

void AEE2001ComfortBus::HandleFeedbackSignal(FeedbackSignal signal)
{
    // React to signals and send immediate messages via the transport layer.
    switch (signal)
    {
        case FeedbackSignal::GetTripComputerData:
        {
            bool emulateDisplay = _carState->EMULATE_DISPLAY_ON_SOURCE;
            bool reverseNotEngaged = (_carState->IsReverseEngaged == 0);
            bool reverseEngagedButParkingAidIsNotVanBusType =
                (_carState->IsReverseEngaged == 1 && _carState->PARKING_AID_TYPE != 0x01);

            if (emulateDisplay && (reverseNotEngaged || reverseEngagedButParkingAidIsNotVanBusType))
            {
                BusMessage msgToSend;
                msgToSend.id = 0x564;
                msgToSend.protocol = ProtocolType::AEE2001;
                msgToSend.type = MessageType::Query;
                _transportLayer->SendMessage(msgToSend, true);
            }
            break;
        }
        case FeedbackSignal::QueryParkingRadarData:
        {
            BusMessage msgToSend;
            msgToSend.id = 0xAE8;
            //msgToSend.id = 0x568;
            msgToSend.protocol = ProtocolType::AEE2001;
            msgToSend.type = MessageType::Query;
            _transportLayer->SendMessage(msgToSend, true);
            break;
        }
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

            _canPopupHandler->SetIgnition(_carState->CurrenTime, _carState->Ignition);
            break;
        }

        default:
            break;
    }
}

void AEE2001ComfortBus::UpdateMessages(uint64_t currentTime)
{
    // Let the scheduler manage periodic sending.
    //printf("AEE2001ComfortBus::UpdateMessages\n");
    _schedulerForSourceNetwork->Update(currentTime, *_transportLayer);
}

void AEE2001ComfortBus::ProcessImmediateSignal(ImmediateSignal signal)
{
    return;
}

bool IRAM_ATTR AEE2001ComfortBus::CanParseMessage(const BusMessage& message)
{
    //printf("AEE2001ComfortBus::CanParseMessage: %X\n", (unsigned int)message.id);
    bool result = _messageHandlers.find(message.id) != _messageHandlers.end();

    //printf("AEE2001ComfortBus::CanParseMessage: %X result: %d\n", (unsigned int)message.id, result);
    return result;
}