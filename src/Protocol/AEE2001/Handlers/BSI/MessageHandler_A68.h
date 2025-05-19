#pragma once

#ifndef _MessageHandler_A68_h
    #define _MessageHandler_A68_h

#include <inttypes.h>
#include <memory>
#include <cstring>
#include <functional>

#include "../../Structs/VAN_8C4.h"
#include "../../../IMessageHandler.hpp"

class MessageHandler_A68 : public IMessageHandler<MessageHandler_A68>
{
    FeedbackSignalCallback _feedbackSignalCallback;
    uint8_t _prevReverseEngaged = 0;
    uint8_t _state = 0;

    public:
        static constexpr uint32_t MessageId = 0xA68;

        MessageHandler_A68()
        {
        }

        void SetFeedbackSignalCallback(FeedbackSignalCallback feedbackSignalCallback) { _feedbackSignalCallback = feedbackSignalCallback; }

        BusMessage Generate(CarState* state)
        {
            if (_prevReverseEngaged == 0 && state->IsReverseEngaged == 1)
            {
                _state = 1;
            }
            _prevReverseEngaged = state->IsReverseEngaged;

            BusMessage message;
            message.id = 0xA68;
            message.periodicityMs = 100;
            message.offsetMs = 20;
            message.protocol = ProtocolType::AEE2001;
            message.type = MessageType::Normal;
            message.ack = true;
            message.isActive = state->PARKING_AID_TYPE == 1 && state->IsReverseEngaged == 1;

            switch (_state)
            {
                case 1:
                    message.data[0] = 0x21;
                    message.data[1] = 0x80;
                    break;
                case 2:
                    message.data[0] = 0x21;
                    message.data[1] = 0xA0;
                break;

                default:
                    break;
            }

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {
            if (message.dataLength != 2)
            {
                return;
            }

            if (message.data[0] == 0x21 && message.data[1] == 0x80)
            {
                _state = 2;
                _feedbackSignalCallback(FeedbackSignal::QueryParkingRadarData);
                return;
            }

            if (message.data[0] == 0x21 && message.data[1] == 0xA0)
            {
                _feedbackSignalCallback(FeedbackSignal::QueryParkingRadarData);
                return;
            }
        }
};
#endif
