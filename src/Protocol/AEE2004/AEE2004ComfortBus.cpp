#include <cstring>
#include <algorithm>
#include <esp_attr.h>
#include "../../Helpers/MessageHandlerTupleTemplates.hpp"

#include "AEE2004ComfortBus.hpp"

AEE2004ComfortBus* AEE2004ComfortBus::_instance = nullptr;

AEE2004ComfortBus::AEE2004ComfortBus(
        CarState* carState,
        ITransportLayer* transport,
        MessageScheduler* scheduler
        )
{
    _instance = this;
    _carState = carState;
    _transportLayer = transport;
    _scheduler = scheduler;
    _feedbackSignalCallback = &FeedbackSignalTrampoline;
    _immediateSignalCallback = nullptr;
}

void AEE2004ComfortBus::RegisterMessageHandlers(ImmediateSignalCallback immediateSignalCallback)
{
    _immediateSignalCallback = immediateSignalCallback;

    std::get<MessageHandler_1A1>(handlers).SetImmediateSignalCallback(_immediateSignalCallback);
    std::get<MessageHandler_1A8>(handlers).SetImmediateSignalCallback(_immediateSignalCallback);
    std::get<MessageHandler_21F>(handlers).SetImmediateSignalCallback(_immediateSignalCallback);
    std::get<MessageHandler_128>(handlers).SetImmediateSignalCallback(_immediateSignalCallback);
    std::get<MessageHandler_168>(handlers).SetImmediateSignalCallback(_immediateSignalCallback);
    std::get<MessageHandler_220>(handlers).SetImmediateSignalCallback(_immediateSignalCallback);
    std::get<MessageHandler_221>(handlers).SetImmediateSignalCallback(_immediateSignalCallback);
    std::get<MessageHandler_217>(handlers).SetImmediateSignalCallback(_immediateSignalCallback);
    std::get<MessageHandler_15B_2004>(handlers).SetImmediateSignalCallback(_immediateSignalCallback);
}

bool IRAM_ATTR AEE2004ComfortBus::ReceiveMessage(BusMessage& message)
{
    message.protocol = ProtocolType::AEE2004;

    bool result = _transportLayer->ReceiveMessage(message);

    return result;
}

void IRAM_ATTR AEE2004ComfortBus::ParseMessage(const BusMessage& message)
{
    if (_carState == nullptr)
    {
        return;
    }
    processHandlersTuple(handlers, _carState, message);
}

void AEE2004ComfortBus::GenerateMessages(MessageDirection direction)
{
    // Generate messages based on the car state and send them via the transport layer.

    if (_carState == nullptr || _scheduler == nullptr)
    {
        return;
    }

    if (direction == MessageDirection::Source)
    {
        //GenerateMessagesForSource();

        //
        /*
        BusMessage msg_036 = { 0x036, 0x00, {0x0E, 0x00, 0x00, 0x21, 0x21, 0x80, 0x00, 0xA0}, 8, 0, false, ProtocolType::AEE2004, MessageType::Normal, 100, true };
        _transportLayer->SendMessage(msg_036);

        uint8_t temperature = _carState->ExternalTemperature;
        BusMessage msg_0f6 = { 0x0F6, 0x00, {0x8E, 0x71, 0x21, 0x8D, 0xF3, 0x65, temperature, 0x50}, 8, 0, false, ProtocolType::AEE2004, MessageType::Normal, 100, true };
        _transportLayer->SendMessage(msg_0f6);
        //*/

        return;
    }

    if (direction == MessageDirection::Destination)
    {
        processGeneratorsTuple(handlers, _carState, _scheduler);
        return;
    }
}

void AEE2004ComfortBus::HandleFeedbackSignal(FeedbackSignal signal)
{
    // React to signals and send immediate messages via the transport layer.
    //printf("AEE2004ComfortBus::HandleFeedbackSignal: %d\n", (int)signal);
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


bool AEE2004ComfortBus::CanAcceptMessage(const BusMessage& message)
{
    if (message.protocol == ProtocolType::AEE2004)
    {
        return true;
    }
    if (message.protocol == ProtocolType::AEE2010)
    {
        switch (message.id)
        {
            case 0x31C:
            case 0x532:
            case 0x5F2:
            return true;
            break;

        default:
            return false;
            break;
        }
    }
    return false;
}

void AEE2004ComfortBus::HandleForwardedMessage(const BusMessage& message)
{
    //printf("AEE2004ComfortBus::HandleForwardedMessage: %X\n", (unsigned int)message.id);
    _transportLayer->SendMessage(message);
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
        case ImmediateSignal::CarSettingsChanged:
        {
            SendImmediateMessage(0x15B);
            break;
        }
        default:
            break;
    }
}

void AEE2004ComfortBus::SendImmediateMessage(uint32_t id)
{
    //printf("AEEE2004 SendImmediateMessage: %X\n", (unsigned int)id);

    std::apply([&](auto&... handler) {
        (..., (std::remove_reference_t<decltype(handler)>::MessageId == id
            ? (
                _scheduler->AddOrUpdateMessage(handler.Generate(_carState), _carState->CurrenTime),
                _scheduler->SendImmedateMessage(id, _carState->CurrenTime, *_transportLayer),
                void()
            )
            : void()));
    }, handlers);
}

bool IRAM_ATTR AEE2004ComfortBus::CanParseMessage(const BusMessage& message)
{
    for (uint32_t id : SupportedMessageIds)
    {
        if (id == message.id)
        {
            return true;
        }
    }
    return false;
}
