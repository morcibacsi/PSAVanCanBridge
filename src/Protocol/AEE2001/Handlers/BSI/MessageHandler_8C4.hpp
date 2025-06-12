#pragma once

#ifndef _MessageHandler_8C4_h
    #define _MessageHandler_8C4_h

#include <cstdint>
#include <cstring>

#include "../../Structs/VAN_8C4.h"
#include "../../../IMessageHandler.hpp"
#include "../../../FeedbackSignal.hpp"

class MessageHandler_8C4 : public IMessageHandler<MessageHandler_8C4>
{
    FeedbackSignalCallback _feedbackSignalCallback;

    public:
        static constexpr uint32_t MessageId = 0x8C4;

        MessageHandler_8C4()
        {
        }

        void SetFeedbackSignalCallback(FeedbackSignalCallback feedbackSignalCallback) { _feedbackSignalCallback = feedbackSignalCallback; }

        BusMessage Generate(CarState* carState)
        {
            BusMessage message;
            message.id = MessageId;
            message.periodicityMs = 500;
            message.offsetMs = 20;
            message.protocol = ProtocolType::AEE2001;
            message.type = MessageType::Normal;
            message.isActive = false;
            message.ack = true;

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {
            VanEventByte1Struct eventSource;
            eventSource.asByte = message.data[0];

            if (eventSource.data.event_src == VAN_EVENT_SRC_BSI)
            {
                VanEventBSIStructs packet;
                std::memcpy(&packet, message.data, sizeof(packet));

                if (packet.Event.asByte > 0)
                {
                    if (_feedbackSignalCallback)
                    {
                        _feedbackSignalCallback(FeedbackSignal::GetTripComputerData);
                    }
                }
            }
        }
};
#endif
