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

    _messageHandlers[0x0B6] = new MessageHandler_0B6_2010();
    _messageHandlers[0x0E6] = new MessageHandler_0E6_2010();
    _messageHandlers[0x0F6] = new MessageHandler_0F6_2010();
    _messageHandlers[0x1A1] = new MessageHandler_1A1_2010();
    _messageHandlers[0x1A8] = new MessageHandler_1A8_2010();
    _messageHandlers[0x2A1] = new MessageHandler_2A1_2010();//
    _messageHandlers[0x2B6] = new MessageHandler_2B6_2010();//vin
    _messageHandlers[0x2E1] = new MessageHandler_2E1_2010();//
    _messageHandlers[0x3B6] = new MessageHandler_3B6_2010();//vin
    _messageHandlers[0x3E7] = new MessageHandler_3E7_2010();
    _messageHandlers[0x10B] = new MessageHandler_10B_2010();
    _messageHandlers[0x21F] = new MessageHandler_21F_2010();
    _messageHandlers[0x036] = new MessageHandler_036_2010();
    _messageHandlers[0x120] = new MessageHandler_120_2010();
    _messageHandlers[0x128] = new MessageHandler_128_2010();
    _messageHandlers[0x161] = new MessageHandler_161_2010();
    _messageHandlers[0x168] = new MessageHandler_168_2010();
    _messageHandlers[0x221] = new MessageHandler_221_2010();
    _messageHandlers[0x227] = new MessageHandler_227_2010();
    _messageHandlers[0x228] = new MessageHandler_228_2010();
    _messageHandlers[0x236] = new MessageHandler_236_2010();
    _messageHandlers[0x260] = new MessageHandler_260_2010();
    _messageHandlers[0x261] = new MessageHandler_261_2010();
    _messageHandlers[0x276] = new MessageHandler_276_2010();
    _messageHandlers[0x336] = new MessageHandler_336_2010();
    _messageHandlers[0x361] = new MessageHandler_361_2010();

    _messageHandlers[0x122] = new MessageHandler_122_2010();
    _messageHandlers[0x217] = new MessageHandler_217_2010();
}

bool AEE2010ComfortBus::ReceiveMessage(BusMessage& message)
{
    message.protocol = ProtocolType::AEE2010;
    return _transportLayer->ReceiveMessage(message);
}

void AEE2010ComfortBus::ParseMessage(const BusMessage& message)
{
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

void AEE2010ComfortBus::GenerateMessages(MessageDirection direction)
{
    if (direction == MessageDirection::Destination)
    {
        for (IMessageHandler* handler : _messageHandlers)
        {
            if (handler != nullptr)
            {
                BusMessage message = handler->Generate(_carState);
                _scheduler->AddOrUpdateMessage(message, _carState->CurrenTime);
            }
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

    IMessageHandler* handler = _messageHandlers[id];

    if (handler == nullptr)
    {
        return;
    }
    BusMessage message = handler->Generate(_carState);
    _scheduler->AddOrUpdateMessage(message, _carState->CurrenTime);

    _scheduler->SendImmedateMessage(id, _carState->CurrenTime, *_transportLayer);
}

bool AEE2010ComfortBus::CanParseMessage(const BusMessage& message)
{
    if (message.id > MAX_CAN_ID)
    {
        return false;
    }

    auto handler = _messageHandlers[message.id];
    bool result = handler != nullptr;

    return result;
}