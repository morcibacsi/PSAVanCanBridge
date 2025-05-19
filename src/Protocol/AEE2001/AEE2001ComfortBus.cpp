#include <string.h>
#include <esp_attr.h>

#include "AEE2001ComfortBus.hpp"

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

    std::get<MessageHandler_4FC>(handlers).SetImmediateSignalCallback(_immediateSignalCallback);

    std::get<MessageHandler_8A4>(handlers).SetImmediateSignalCallback(_immediateSignalCallback);
    std::get<MessageHandler_8A4>(handlers).SetFeedbackSignalCallback(_feedbackSignalCallback);
    std::get<MessageHandler_8A4>(handlers).SetCanDisplayPopupHandler(_canPopupHandler);

    std::get<MessageHandler_8C4>(handlers).SetFeedbackSignalCallback(_feedbackSignalCallback);

    std::get<MessageHandler_9C4>(handlers).SetImmediateSignalCallback(_immediateSignalCallback);

    std::get<MessageHandler_524>(handlers).SetCanDisplayPopupHandler(_canPopupHandler);

    std::get<MessageHandler_564>(handlers).SetCanDisplayPopupHandler(_canPopupHandler);
    std::get<MessageHandler_564>(handlers).SetImmediateSignalCallback(_immediateSignalCallback);

    std::get<MessageHandler_824>(handlers).SetImmediateSignalCallback(_immediateSignalCallback);

    std::get<MessageHandler_A68>(handlers).SetFeedbackSignalCallback(_feedbackSignalCallback);
}

bool AEE2001ComfortBus::ReceiveMessage(BusMessage& message)
{
    message.protocol = ProtocolType::AEE2001;
    return _transportLayer->ReceiveMessage(message);
}

void AEE2001ComfortBus::ParseMessage(const BusMessage& message)
{
    std::apply([&](auto&... handler)
    {
        (..., (std::remove_reference_t<decltype(handler)>::MessageId == message.id
            ? (handler.Parse(_carState, message), void())
            : void()
        ));
    }, handlers);
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
    BusMessage aasMessage = std::get<MessageHandler_A68>(handlers).Generate(_carState);;
    _schedulerForSourceNetwork->AddOrUpdateMessage(aasMessage, _carState->CurrenTime);

    BusMessage emfMessage = std::get<MessageHandler_5E4>(handlers).Generate(_carState);
    _schedulerForSourceNetwork->AddOrUpdateMessage(emfMessage, _carState->CurrenTime);
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
            if (_carState->IsReverseEngaged)
            {
                auto aasHandler = std::get<MessageHandler_A68>(handlers);
                auto message = aasHandler.Generate(_carState);

                if (message.isActive)
                {
                    _transportLayer->SendMessage(message, true);
                }
            }

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
    for (uint32_t id : SupportedMessageIds)
    {
        if (id == message.id)
        {
            return true;
        }
    }
    return false;
}