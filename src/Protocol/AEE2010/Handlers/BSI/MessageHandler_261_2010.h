#pragma once

#ifndef _MessageHandler_261_2010_h
    #define _MessageHandler_261_2010_h

#include <inttypes.h>
#include <memory>
#include <cstring>

#include "../../../IMessageHandler.hpp"

class MessageHandler_261_2010 : public IMessageHandler<MessageHandler_261_2010>
{
    private:
        BusMessage message
        {
            .id = 0x261,
            .data = {0},
            .dataLength = 7,
            .protocol = ProtocolType::AEE2010,
            .periodicityMs = 1000-150,
            .offsetMs = 730,
            .isActive = true
        };
    public:
        static constexpr uint32_t MessageId = 0x261;

        BusMessage Generate(CarState* carState)
        {
            message.data[0] = carState->Trip2Speed;//AverageSpeed
            message.data[1] = carState->Trip2Distance.data.leftByte;
            message.data[2] = carState->Trip2Distance.data.rightByte;
            message.data[3] = carState->Trip2Consumption.data.leftByte;//Consumption
            message.data[4] = carState->Trip2Consumption.data.rightByte;
            message.data[5] = carState->Trip2LastResetSince.data.leftByte;
            message.data[6] = carState->Trip2LastResetSince.data.rightByte;

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {

        }
};
#endif
