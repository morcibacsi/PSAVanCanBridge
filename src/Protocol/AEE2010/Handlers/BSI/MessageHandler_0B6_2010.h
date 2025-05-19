#pragma once

#ifndef _MessageHandler_0B6_2010_h
    #define _MessageHandler_0B6_2010_h

#include <inttypes.h>
#include <memory>
#include "../../../IMessageHandler.hpp"

class MessageHandler_0B6_2010 : public IMessageHandler<MessageHandler_0B6_2010>
{
    private:
        BusMessage message
        {
            .id = 0x0B6,
            .data = {0},
            .dataLength = 8,
            .protocol = ProtocolType::AEE2010,
            .periodicityMs = 100,
            .offsetMs = 20,
            .isActive = true
        };

    public:
        static constexpr uint32_t MessageId = 0x0B6;

        BusMessage Generate(CarState* state)
        {
            message.data[0] = state->Rpm.data.leftByte;
            message.data[1] = state->Rpm.data.rightByte;
            message.data[2] = state->Speed.data.leftByte;
            message.data[3] = state->Speed.data.rightByte;
            message.data[4] = state->DistanceForCMB.data.leftByte;
            message.data[5] = state->DistanceForCMB.data.rightByte;
            message.data[6] = state->ConsumptionForCMB;
            message.data[7] = 0xD0;

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {

        }
};
#endif
