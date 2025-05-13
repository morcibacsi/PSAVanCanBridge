#pragma once

#ifndef _MessageHandler_2A1_2004_h
    #define _MessageHandler_2A1_2004_h

#include <inttypes.h>
#include <memory>
#include <cstring>

#include "../../../IMessageHandler.hpp"

class MessageHandler_2A1 : public IMessageHandler
{
    private:
        BusMessage message
        {
            .id = 0x2A1,
            .data = {0},
            .dataLength = 7,
            .protocol = ProtocolType::AEE2004,
            .periodicityMs = 1000-250,
            .offsetMs = 930,
            .isActive = true
        };
    public:
        BusMessage Generate(std::shared_ptr<CarState> state) override
        {
            message.data[0] = state->Trip1Speed;
            message.data[1] = state->Trip1Distance.data.leftByte;
            message.data[2] = state->Trip1Distance.data.rightByte;
            message.data[3] = state->Trip1Consumption.data.leftByte;
            message.data[4] = state->Trip1Consumption.data.rightByte;
            message.data[5] = 0x00;
            message.data[6] = 0x00;

            return message;
        }

        void Parse(std::shared_ptr<CarState> carState, const BusMessage& message) override
        {
            if (message.dataLength < 7)
            {
                return;
            }
            carState->Trip1Speed                         = message.data[0]; // AverageSpeed
            carState->Trip1Distance.data.leftByte        = message.data[1]; // Distance
            carState->Trip1Distance.data.rightByte       = message.data[2];
            carState->Trip1Consumption.data.leftByte     = message.data[3]; // Consumption
            carState->Trip1Consumption.data.rightByte    = message.data[4];
            carState->Trip1LastResetSince.data.leftByte  = message.data[5];
            carState->Trip1LastResetSince.data.rightByte = message.data[6];
        }
};
#endif
