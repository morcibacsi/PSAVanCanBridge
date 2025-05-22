#pragma once

#ifndef _MessageHandler_0E1_2010_h
    #define _MessageHandler_0E1_2010_h

#include <cstdint>
#include <cstring>

#include "../../../IMessageHandler.hpp"

class MessageHandler_0E1_2010 : public IMessageHandler<MessageHandler_0E1_2010>
{
    private:
        BusMessage message
        {
            .id = 0x0E1,
            .data = {0},
            .dataLength = 7,
            .protocol = ProtocolType::AEE2010,
            .periodicityMs = 100,
            .offsetMs = 0,
            .isActive = true
        };
    public:
        static constexpr uint32_t MessageId = 0x0E1;

        BusMessage Generate(CarState* carState);
        void Parse(CarState* carState, const BusMessage& message);
};
#endif
