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

    _messageHandlers[0x0E1] = new MessageHandler_0E1();

    _messageHandlers[0x0B6] = new MessageHandler_0B6();
    _messageHandlers[0x0E6] = new MessageHandler_0E6();
    _messageHandlers[0x0E8] = new MessageHandler_0E8();
    _messageHandlers[0x0F6] = new MessageHandler_0F6();
    _messageHandlers[0x1A1] = new MessageHandler_1A1(_immediateSignalCallback);
    _messageHandlers[0x1A8] = new MessageHandler_1A8(_immediateSignalCallback);
    _messageHandlers[0x2A1] = new MessageHandler_2A1();
    _messageHandlers[0x2B6] = new MessageHandler_2B6();
    _messageHandlers[0x2E1] = new MessageHandler_2E1();
    _messageHandlers[0x3A7] = new MessageHandler_3A7();
    _messageHandlers[0x3B6] = new MessageHandler_3B6();
    _messageHandlers[0x10B] = new MessageHandler_10B();
    _messageHandlers[0x21F] = new MessageHandler_21F(_immediateSignalCallback);
    _messageHandlers[0x036] = new MessageHandler_036();
    _messageHandlers[0x120] = new MessageHandler_120();
    _messageHandlers[0x127] = new MessageHandler_127();
    _messageHandlers[0x128] = new MessageHandler_128(_immediateSignalCallback);
    _messageHandlers[0x161] = new MessageHandler_161();
    _messageHandlers[0x168] = new MessageHandler_168(_immediateSignalCallback);
    _messageHandlers[0x220] = new MessageHandler_220(_immediateSignalCallback);
    _messageHandlers[0x221] = new MessageHandler_221(_immediateSignalCallback);
    _messageHandlers[0x227] = new MessageHandler_227();
    _messageHandlers[0x261] = new MessageHandler_261();
    _messageHandlers[0x336] = new MessageHandler_336();
    _messageHandlers[0x361] = new MessageHandler_361();

    _messageHandlers[0x1E3] = new MessageHandler_1E3();

    _messageHandlers[0x217] = new MessageHandler_217(_immediateSignalCallback);

    //_messageHandlers[0x167] = new MessageHandler_167();
    //_messageHandlers[0x297] = new MessageHandler_297();
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
    //printf("AEE2004 ParseMessage: %X\n", (unsigned int)message.id);

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

    IMessageHandler* handler = _messageHandlers[id];

    if (handler == nullptr)
    {
        return;
    }
    BusMessage message = handler->Generate(_carState);
    _scheduler->AddOrUpdateMessage(message, _carState->CurrenTime);

    _scheduler->SendImmedateMessage(id, _carState->CurrenTime, *_transportLayer);
}

bool IRAM_ATTR AEE2004ComfortBus::CanParseMessage(const BusMessage& message)
{
    auto handler = _messageHandlers[message.id];
    bool result = handler != nullptr;

    return result;
}