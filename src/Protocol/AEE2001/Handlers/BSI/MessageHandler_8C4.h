#pragma once

#ifndef _MessageHandler_8C4_h
    #define _MessageHandler_8C4_h

#include <inttypes.h>
#include <memory>
#include <cstring>
#include <functional>

#include "../../Structs/VAN_8C4.h"
#include "../../../IMessageHandler.hpp"

class MessageHandler_8C4 : public IMessageHandler
{
    std::function<void(FeedbackSignal)> _feedbackSignalCallback;

    public:
        MessageHandler_8C4(
            std::function<void(FeedbackSignal)> feedbackSignalCallback
        )
        {
            _feedbackSignalCallback = feedbackSignalCallback;
        }

        BusMessage Generate(std::shared_ptr<CarState> state) override
        {
            BusMessage message;
            message.id = 0x8C4;
            message.periodicityMs = 500;
            message.offsetMs = 20;
            message.protocol = ProtocolType::AEE2001;

            return message;
        }

        void Parse(std::shared_ptr<CarState> carState, const BusMessage& message) override
        {
            VanEventByte1Struct eventSource;
            eventSource.asByte = message.data[0];

            if (eventSource.data.event_src == VAN_EVENT_SRC_BSI)
            {
                VanEventBSIStructs vanPacket;
                std::memcpy(&vanPacket, message.data, sizeof(vanPacket));

                //if (vanPacket.Event.data.trip_change_status == 1 || vanPacket.Event.data.door_change_status == 1)
                if (vanPacket.Event.asByte > 0)
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
