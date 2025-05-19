#pragma once

#ifndef _MessageHandler_227_2010_h
    #define _MessageHandler_227_2010_h

#include <inttypes.h>
#include <memory>
#include "../../../IMessageHandler.hpp"

class MessageHandler_227_2010 : public IMessageHandler<MessageHandler_227_2010>
{
    private:
        BusMessage message
        {
            .id = 0x227,
            .data = {0},
            .dataLength = 8,
            .protocol = ProtocolType::AEE2010,
            .periodicityMs = 500,
            .offsetMs = 140,
            .isActive = true

        };
    public:
        static constexpr uint32_t MessageId = 0x227;

        BusMessage Generate(CarState* state)
        {
            message.data[0] = state->CarLedStatusByte1.asByte;
            message.data[1] = state->CarLedStatusByte2.asByte;
            message.data[2] = state->CarLedStatusByte3.asByte;
            message.data[3] = state->CarLedStatusByte4.asByte;
            message.data[4] = 0x00;
            message.data[5] = 0x00;
            message.data[6] = 0x00;
            message.data[7] = 0x00;

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {

        }
};
#endif
