#include <algorithm>
#include <esp_attr.h>

#include "AEE2010ComfortBus.hpp"

AEE2010ComfortBus::AEE2010ComfortBus(
        CarState* carState,
        ITransportLayer* transport,
        MessageScheduler* scheduler
        )
{
    _carState = carState;
    _transportLayer = transport;
    _scheduler = scheduler;
    _messagesToSkip = std::vector<uint32_t>() = {
        0x0B6, 0x0E6, 0x0F6, 0x1A1, 0x1A8,
        0x2B6, 0x3B6, 0x3E7, 0x21F, 0x036,
        0x122, 0x128, 0x161, 0x168, 0x217,
        0x220, 0x221, 0x227, 0x228, 0x236,
        0x260, 0x276, 0x336, 0x361, 0x321,
        0x3A7, 0x127, 0x0E8, 0x3F6, 0x061,
        0x1E8, 0x2D7, 0x1CC
    };
    _messagesToForward = std::vector<uint32_t>() = {
        0x0E1, 0x1E1, 0x1E7, 0x1E8, 0x297,
        0x3E0, 0x3E1, 0x120, 0x2A0, 0x0A2
    };
}

void AEE2010ComfortBus::RegisterMessageHandlers(ImmediateSignalCallback immediateSignalCallback)
{
    _immediateSignalCallback = immediateSignalCallback;
}

bool AEE2010ComfortBus::ReceiveMessage(BusMessage& message)
{
    message.protocol = ProtocolType::AEE2010;
    return _transportLayer->ReceiveMessage(message);
}

void AEE2010ComfortBus::ParseMessage(const BusMessage& message)
{
    std::apply([&](auto&... handler)
    {
        (..., (std::remove_reference_t<decltype(handler)>::MessageId == message.id
            ? (handler.Parse(_carState, message), void())
            : void()
        ));
    }, handlers);
}

void AEE2010ComfortBus::GenerateMessages(MessageDirection direction)
{
    std::apply([&](auto&... handler)
    {
        (..., [&]
            {
                BusMessage msg = handler.Generate(_carState);
                _scheduler->AddOrUpdateMessage(msg, _carState->CurrenTime);
            }());
    }, handlers);
}

void AEE2010ComfortBus::HandleFeedbackSignal(FeedbackSignal signal)
{
    // React to signals and send immediate messages via the transport layer.
    /*
    if (signal == FeedbackSignal::GetTripComputerData)
    {
        BusMessage immediateMsg;
        immediateMsg.id = 0x400;
        immediateMsg.data.push_back(0x01);
        _transportLayer->SendMessage(immediateMsg);
    }
    //*/
}

bool AEE2010ComfortBus::CanAcceptMessage(const BusMessage& message)
{
    if (message.protocol == ProtocolType::AEE2010)
    {
        return true;
    }
    if (message.protocol == ProtocolType::AEE2004)
    {
        switch (message.id)
        {
            case 0x0E1:
            case 0x1E1:
            case 0x1E7:
            case 0x1E8:
            case 0x297:
            case 0x3E0:
            case 0x3E1:
            case 0x120:
            case 0x2A0:
            case 0x0A2:
            return true;
            break;

        default:
            return false;
            break;
        }
        /*
        if (std::find(_messagesToSkip.begin(), _messagesToSkip.end(), message.id) != _messagesToSkip.end())
        {
            return false;
        }
        if (std::find(_messagesToForward.begin(), _messagesToForward.end(), message.id) != _messagesToForward.end())
        {
            return true;
        }
        */
    }
    return false;
}

void AEE2010ComfortBus::HandleForwardedMessage(const BusMessage& message)
{
    //printf("AEE2010ComfortBus::HandleForwardedMessage: %X\n", (unsigned int)message.id);
    _transportLayer->SendMessage(message);
}

void AEE2010ComfortBus::UpdateMessages(uint64_t currentTime)
{
    // Let the scheduler manage periodic sending.
    _scheduler->Update(currentTime, *_transportLayer);
}

void AEE2010ComfortBus::ProcessImmediateSignal(ImmediateSignal signal)
{
    ///*
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
            break;
        }
        case ImmediateSignal::SpeedAndRpm:
        {
            SendImmediateMessage(0x0B6);
            SendImmediateMessage(0x297);
            break;
        }
        case ImmediateSignal::PopupMessage:
        {
            SendImmediateMessage(0x1A1);
            break;
        }
        case ImmediateSignal::CmbSignalLightsChanged:
        {
            SendImmediateMessage(0x128);
            break;
        }
        case ImmediateSignal::CmbIndicatorLightsChanged:
        {
            SendImmediateMessage(0x168);
            break;
        }
        case ImmediateSignal::CmbStatusChanged:
        {
            SendImmediateMessage(0x217);
            break;
        }
        default:
        break;
    }
    //*/
}

void AEE2010ComfortBus::SendImmediateMessage(uint32_t id)
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

bool AEE2010ComfortBus::CanParseMessage(const BusMessage& message)
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