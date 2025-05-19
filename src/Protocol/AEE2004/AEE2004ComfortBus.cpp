#include "AEE2004ComfortBus.hpp"

#include <cstring>
#include <algorithm>
#include <esp_attr.h>

AEE2004ComfortBus::AEE2004ComfortBus(
        CarState* carState,
        ITransportLayer* transport,
        MessageScheduler* scheduler
        )
{
    _carState = carState;
    _transportLayer = transport;
    _scheduler = scheduler;
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
}

bool IRAM_ATTR AEE2004ComfortBus::ReceiveMessage(BusMessage& message)
{
    message.protocol = ProtocolType::AEE2004;

    bool result = _transportLayer->ReceiveMessage(message);

    return result;
}

void IRAM_ATTR AEE2004ComfortBus::ParseMessage(const BusMessage& message)
{
    //search for the message id in the map and if exists call the parse function
    //printf("AEE2004 ParseMessage: %X\n", (unsigned int)message.id);

    std::apply([&](auto&... handler)
    {
        (..., (std::remove_reference_t<decltype(handler)>::MessageId == message.id
            ? (handler.Parse(_carState, message), void())
            : void()
        ));
    }, handlers);
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
        std::apply([&](auto&... handler)
        {
            (..., [&]
            {
                BusMessage msg = handler.Generate(_carState);
                _scheduler->AddOrUpdateMessage(msg, _carState->CurrenTime);
            }());
        }, handlers);
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