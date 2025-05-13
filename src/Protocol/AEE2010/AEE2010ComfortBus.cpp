#include <algorithm>
#include <esp_attr.h>

#include "AEE2010ComfortBus.hpp"

#include "Handlers/BSI/MessageHandler_0B6_2010.h"
#include "Handlers/BSI/MessageHandler_0E6_2010.h"
#include "Handlers/BSI/MessageHandler_0F6_2010.h"
#include "Handlers/BSI/MessageHandler_1A1_2010.h"
#include "Handlers/BSI/MessageHandler_1A8_2010.h"
#include "Handlers/BSI/MessageHandler_2B6_2010.h"
#include "Handlers/BSI/MessageHandler_2E1_2010.h"
#include "Handlers/BSI/MessageHandler_3B6_2010.h"
#include "Handlers/BSI/MessageHandler_3E7_2010.h"
#include "Handlers/BSI/MessageHandler_10B_2010.h"
#include "Handlers/BSI/MessageHandler_21F_2010.h"
#include "Handlers/BSI/MessageHandler_036_2010.h"
#include "Handlers/BSI/MessageHandler_120_2010.h"
#include "Handlers/BSI/MessageHandler_128_2010.h"
#include "Handlers/BSI/MessageHandler_161_2010.h"
#include "Handlers/BSI/MessageHandler_168_2010.h"
#include "Handlers/BSI/MessageHandler_2A1_2010.h"
#include "Handlers/BSI/MessageHandler_221_2010.h"
#include "Handlers/BSI/MessageHandler_227_2010.h"
#include "Handlers/BSI/MessageHandler_228_2010.h"
#include "Handlers/BSI/MessageHandler_236_2010.h"
#include "Handlers/BSI/MessageHandler_260_2010.h"
#include "Handlers/BSI/MessageHandler_261_2010.h"
#include "Handlers/BSI/MessageHandler_276_2010.h"
#include "Handlers/BSI/MessageHandler_336_2010.h"
#include "Handlers/BSI/MessageHandler_361_2010.h"

#include "Handlers/CMB/MessageHandler_122_2010.h"
#include "Handlers/CMB/MessageHandler_217_2010.h"

AEE2010ComfortBus::AEE2010ComfortBus(
        std::shared_ptr<CarState> carState,
        std::shared_ptr<ITransportLayer> transport,
        std::shared_ptr<MessageScheduler> scheduler
        )
{
    _carState = std::move(carState);
    _transportLayer = std::move(transport);
    _scheduler = std::move(scheduler);
    _messageHandlers = std::unordered_map<uint32_t, std::unique_ptr<IMessageHandler>>();
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
        0x3E0, 0x3E1, 0x120, 0x2A0, 0x0A2,
        0x2A0
    };
}

void AEE2010ComfortBus::RegisterMessageHandlers(std::function<void(ImmediateSignal)> immediateSignalCallback)
{
    _messageHandlers.insert({0x0B6, std::make_unique<MessageHandler_0B6_2010>()});
    _messageHandlers.insert({0x0E6, std::make_unique<MessageHandler_0E6_2010>()});
    _messageHandlers.insert({0x0F6, std::make_unique<MessageHandler_0F6_2010>()});
    _messageHandlers.insert({0x1A1, std::make_unique<MessageHandler_1A1_2010>()});
    _messageHandlers.insert({0x1A8, std::make_unique<MessageHandler_1A8_2010>()});
    _messageHandlers.insert({0x2A1, std::make_unique<MessageHandler_2A1_2010>()});//
    _messageHandlers.insert({0x2B6, std::make_unique<MessageHandler_2B6_2010>()});//vin
    _messageHandlers.insert({0x2E1, std::make_unique<MessageHandler_2E1_2010>()});//
    _messageHandlers.insert({0x3B6, std::make_unique<MessageHandler_3B6_2010>()});//vin
    _messageHandlers.insert({0x3E7, std::make_unique<MessageHandler_3E7_2010>()});
    _messageHandlers.insert({0x10B, std::make_unique<MessageHandler_10B_2010>()});
    _messageHandlers.insert({0x21F, std::make_unique<MessageHandler_21F_2010>()});
    _messageHandlers.insert({0x036, std::make_unique<MessageHandler_036_2010>()});
    _messageHandlers.insert({0x120, std::make_unique<MessageHandler_120_2010>()});
    _messageHandlers.insert({0x128, std::make_unique<MessageHandler_128_2010>()});
    _messageHandlers.insert({0x161, std::make_unique<MessageHandler_161_2010>()});
    _messageHandlers.insert({0x168, std::make_unique<MessageHandler_168_2010>()});
    _messageHandlers.insert({0x221, std::make_unique<MessageHandler_221_2010>()});
    _messageHandlers.insert({0x227, std::make_unique<MessageHandler_227_2010>()});
    _messageHandlers.insert({0x228, std::make_unique<MessageHandler_228_2010>()});
    _messageHandlers.insert({0x236, std::make_unique<MessageHandler_236_2010>()});
    _messageHandlers.insert({0x260, std::make_unique<MessageHandler_260_2010>()});
    _messageHandlers.insert({0x261, std::make_unique<MessageHandler_261_2010>()});
    _messageHandlers.insert({0x276, std::make_unique<MessageHandler_276_2010>()});
    _messageHandlers.insert({0x336, std::make_unique<MessageHandler_336_2010>()});
    _messageHandlers.insert({0x361, std::make_unique<MessageHandler_361_2010>()});

    _messageHandlers.insert({0x122, std::make_unique<MessageHandler_122_2010>()});
    _messageHandlers.insert({0x217, std::make_unique<MessageHandler_217_2010>()});
}

bool AEE2010ComfortBus::ReceiveMessage(BusMessage& message)
{
    message.protocol = ProtocolType::AEE2010;
    return _transportLayer->ReceiveMessage(message);
}

void AEE2010ComfortBus::ParseMessage(const BusMessage& message)
{
    // Decode the generic BusMessage and update the state.
    if (message.id == 0x100) {
        //state.speed = message.data[0] | (message.data[1] << 8);
    }
}

void AEE2010ComfortBus::GenerateMessages(MessageDirection direction)
{
    if (direction == MessageDirection::Destination)
    {
        for (auto& [id, generator] : _messageHandlers)
        {
            BusMessage message = generator->Generate(_carState);
            _scheduler->AddOrUpdateMessage(message, _carState->CurrenTime);
        }
        return;
    }
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
        if (std::find(_messagesToSkip.begin(), _messagesToSkip.end(), message.id) != _messagesToSkip.end())
        {
            return false;
        }
        if (std::find(_messagesToForward.begin(), _messagesToForward.end(), message.id) != _messagesToForward.end())
        {
            return true;
        }
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

bool AEE2010ComfortBus::CanParseMessage(const BusMessage& message)
{
    return _messageHandlers.find(message.id) != _messageHandlers.end();
}