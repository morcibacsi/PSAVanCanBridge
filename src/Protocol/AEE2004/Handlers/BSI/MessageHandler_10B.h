#pragma once

#ifndef _MessageHandler_10B_h
    #define _MessageHandler_10B_h

#include <inttypes.h>
#include <memory>
#include "../../../IMessageHandler.hpp"
#include "../../Structs/CAN_10B.h"

class MessageHandler_10B : public IMessageHandler<MessageHandler_10B>
{
    private:
        BusMessage message
        {
            .id = 0x10B,
            .data = { 0 },
            .dataLength = 7,
            .protocol = ProtocolType::AEE2004,
            .periodicityMs = 100,
            .offsetMs = 0,
            .isActive = true
        };
    public:
        static constexpr uint32_t MessageId = 0x10B;

        BusMessage Generate(CarState* state)
        {
            message.data[0] = state->SteeringAngle.data.leftByte;
            message.data[1] = state->SteeringAngle.data.rightByte;
            message.data[2] = state->SteeringSpeed;
            message.data[3] = state->SteeringStatus;
            message.data[4] = 0x00;
            message.data[5] = state->SteeringSpeed;
            message.data[6] = 0x00;

            return message;
        }

        void Parse(CarState* state, const BusMessage& msg)
        {
        }
};
#endif
