#pragma once

#ifndef _MessageHandler_2A1_2010_h
    #define _MessageHandler_2A1_2010_h

#include <inttypes.h>
#include <memory>
#include <cstring>

#include "../../../IMessageHandler.hpp"

class MessageHandler_2A1_2010 : public IMessageHandler<MessageHandler_2A1_2010>
{
    private:
    BusMessage message
    {
        .id = 0x2A1,
        .data = {0},
        .dataLength = 7,
        .protocol = ProtocolType::AEE2010,
        .periodicityMs = 1000-150,
        .offsetMs = 930,
        .isActive = true
    };
    public:
        static constexpr uint32_t MessageId = 0x2A1;

        BusMessage Generate(CarState* carState)
        {
            message.data[0] = carState->Trip1Speed;
            message.data[1] = carState->Trip1Distance.data.leftByte;
            message.data[2] = carState->Trip1Distance.data.rightByte;
            message.data[3] = carState->Trip1Consumption.data.leftByte;
            message.data[4] = carState->Trip1Consumption.data.rightByte;
            message.data[5] = carState->Trip1LastResetSince.data.leftByte;
            message.data[6] = carState->Trip1LastResetSince.data.rightByte;

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {

        }
};
#endif
