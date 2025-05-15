#pragma once

#ifndef _MessageHandler_2A1_2010_h
    #define _MessageHandler_2A1_2010_h

#include <inttypes.h>
#include <memory>
#include <cstring>

#include "../../../IMessageHandler.hpp"

class MessageHandler_2A1_2010 : public IMessageHandler
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
        BusMessage Generate(CarState* state) override
        {
            message.data[0] = state->Trip1Speed;
            message.data[1] = state->Trip1Distance.data.leftByte;
            message.data[2] = state->Trip1Distance.data.rightByte;
            message.data[3] = state->Trip1Consumption.data.leftByte;
            message.data[4] = state->Trip1Consumption.data.rightByte;
            message.data[5] = state->Trip1LastResetSince.data.leftByte;
            message.data[6] = state->Trip1LastResetSince.data.rightByte;

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message) override
        {

        }
};
#endif
