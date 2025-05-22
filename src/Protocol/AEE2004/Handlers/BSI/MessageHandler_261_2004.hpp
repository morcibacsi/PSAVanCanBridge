#pragma once

#ifndef _MessageHandler_261_2004_h
    #define _MessageHandler_261_2004_h

#include <cstdint>
#include <cstring>

#include "../../../IMessageHandler.hpp"

class MessageHandler_261 : public IMessageHandler<MessageHandler_261>
{
    private:
        BusMessage message
        {
            .id = 0x261,
            .data = {0},
            .dataLength = 7,
            .protocol = ProtocolType::AEE2004,
            .periodicityMs = 1000-250,
            .offsetMs = 730,
            .isActive = true
        };

    public:
        static constexpr uint32_t MessageId = 0x261;

        BusMessage Generate(CarState* carState)
        {
            message.data[0] = carState->Trip2Speed; // AverageSpeed
            message.data[1] = carState->Trip2Distance.data.leftByte; // Distance
            message.data[2] = carState->Trip2Distance.data.rightByte;
            message.data[3] = carState->Trip2Consumption.data.leftByte; // Consumption
            message.data[4] = carState->Trip2Consumption.data.rightByte;
            message.data[5] = 0x00;
            message.data[6] = 0x00;

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {
            if (message.dataLength < 7)
            {
                return;
            }
            carState->Trip2Speed                         = message.data[0]; // AverageSpeed
            carState->Trip2Distance.data.leftByte        = message.data[1]; // Distance
            carState->Trip2Distance.data.rightByte       = message.data[2];
            carState->Trip2Consumption.data.leftByte     = message.data[3]; // Consumption
            carState->Trip2Consumption.data.rightByte    = message.data[4];
            carState->Trip2LastResetSince.data.leftByte  = message.data[5];
            carState->Trip2LastResetSince.data.rightByte = message.data[6];
        }
};
#endif
