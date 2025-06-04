#pragma once

#ifndef _MessageHandler_321_2010_h
    #define _MessageHandler_321_2010_h

#include <cstdint>
#include <cstring>

#include "../../../IMessageHandler.hpp"

class MessageHandler_321_2010 : public IMessageHandler<MessageHandler_321_2010>
{
    private:
        BusMessage message
        {
            .id = MessageId,
            .data = {0},
            .dataLength = 6,
            .protocol = ProtocolType::AEE2010,
            .periodicityMs = 200-10,
            .offsetMs = 10,
            .isActive = true
        };
    public:
        static constexpr uint32_t MessageId = 0x321;

        BusMessage Generate(CarState* carState)
        {
            message.data[0] = 0x00;
            message.data[1] = 0x00;
            message.data[2] = 0x00;
            message.data[3] = 0x00;
            message.data[4] = 0x00;
            message.data[5] = 0x00;

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {

        }
};
#endif
