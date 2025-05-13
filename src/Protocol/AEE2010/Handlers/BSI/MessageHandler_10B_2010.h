#pragma once

#ifndef _MessageHandler_10B_2010_h
    #define _MessageHandler_10B_2010_h

#include <inttypes.h>
#include <memory>
#include "../../../IMessageHandler.hpp"
#include "../../Structs/CAN_10B_2010.h"

class MessageHandler_10B_2010 : public IMessageHandler
{
    private:
        BusMessage message
        {
            .id = 0x10B,
            .data = {0},
            .dataLength = 7,
            .crc = 0,
            .protocol = ProtocolType::AEE2010,
            .periodicityMs = 100,
            .offsetMs = 0,
            .isActive = true
        };

    public:
        BusMessage Generate(std::shared_ptr<CarState> state) override
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
};
#endif
