#pragma once

#ifndef _MessageHandler_10B_h
    #define _MessageHandler_10B_h

#include <cstdint>

#include "../../../IMessageHandler.hpp"
#include "../../Structs/CAN_10B_2004.h"

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

        BusMessage Generate(CarState* carState)
        {
            message.data[0] = carState->SteeringAngle.data.leftByte;
            message.data[1] = carState->SteeringAngle.data.rightByte;
            message.data[2] = carState->SteeringSpeed;
            message.data[3] = carState->SteeringStatus;
            message.data[4] = 0x00;
            message.data[5] = carState->SteeringSpeed;
            message.data[6] = 0x00;

            return message;
        }

        void Parse(CarState* carState, const BusMessage& msg)
        {
        }
};
#endif
