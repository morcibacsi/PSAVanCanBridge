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

AEE2001ComfortBus* AEE2001ComfortBus::_instance = nullptr;

AEE2001ComfortBus::AEE2001ComfortBus(
    CarState* carState,
    ITransportLayer* transport,
    MessageScheduler* scheduler
    )
{
    _instance = this;
    _transportLayer = transport;
    _schedulerForSourceNetwork = scheduler;
    _carState = carState;

    _canPopupHandler = new CanDisplayPopupHandler3(_carState);
}

void AEE2001ComfortBus::RegisterMessageHandlers(ImmediateSignalCallback immediateSignalCallback)
{
    _feedbackSignalCallback = &FeedbackSignalTrampoline;;
    _immediateSignalCallback = immediateSignalCallback;

    if (_carState->EMULATE_DISPLAY_ON_SOURCE)
    {
        _messageHandlersForSource[0x5E4] = new MessageHandler_5E4();
    }

    if (_carState->PARKING_AID_TYPE == 0x01)
    {
        auto _aasHandler = new MessageHandler_A68(_feedbackSignalCallback);
        _messageHandlersForSource[0xA68] = _aasHandler;
        _messageHandlers[0xA68] = _aasHandler;

        _messageHandlers[0xAE8] = new MessageHandler_AE8();
    }

    _messageHandlers[0x4DC] = new MessageHandler_4DC();
    _messageHandlers[0x4FC] = new MessageHandler_4FC(_immediateSignalCallback);
    _messageHandlers[0x8A4] = new MessageHandler_8A4(_canPopupHandler, _immediateSignalCallback, _feedbackSignalCallback);
    _messageHandlers[0x8C4] = new MessageHandler_8C4(_feedbackSignalCallback);
    _messageHandlers[0x9C4] = new MessageHandler_9C4(_immediateSignalCallback);
    _messageHandlers[0x524] = new MessageHandler_524(_canPopupHandler);
    _messageHandlers[0x564] = new MessageHandler_564(_canPopupHandler, _immediateSignalCallback);
    _messageHandlers[0x744] = new MessageHandler_744();
    _messageHandlers[0x824] = new MessageHandler_824(_immediateSignalCallback);
    _messageHandlers[0x464] = new MessageHandler_464();
}

bool AEE2001ComfortBus::ReceiveMessage(BusMessage& message)
{
    message.protocol = ProtocolType::AEE2001;
    return _transportLayer->ReceiveMessage(message);
}

void AEE2001ComfortBus::ParseMessage(const BusMessage& message)
{
    //printf("AEE2001ComfortBus::ParseMessage: %X\n", (unsigned int)message.id);

    if (message.id > MAX_CAN_ID)
    {
        return;
    }

    auto handler = _messageHandlers[message.id];

    if (handler == nullptr)
    {
        return;
    }
    handler->Parse(_carState, message);
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
    for (IMessageHandler* handler : _messageHandlersForSource)
    {
        if (handler != nullptr)
        {
            BusMessage message = handler->Generate(_carState);
            _schedulerForSourceNetwork->AddOrUpdateMessage(message, _carState->CurrenTime);
        }
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
    auto handler = _messageHandlers[message.id];

    bool result = handler != nullptr;

    //printf("AEE2001ComfortBus::CanParseMessage: %X result: %d\n", (unsigned int)message.id, result);
    return result;
}