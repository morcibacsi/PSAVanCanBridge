#pragma once

#ifndef _MessageHandler_261_2010_h
    #define _MessageHandler_261_2010_h

#include <inttypes.h>
#include <memory>
#include <cstring>

#include "../../../IMessageHandler.hpp"

class MessageHandler_261_2010 : public IMessageHandler
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
        BusMessage Generate(CarState* state) override
        {
            message.data[0] = state->Trip2Speed;//AverageSpeed
            message.data[1] = state->Trip2Distance.data.leftByte;
            message.data[2] = state->Trip2Distance.data.rightByte;
            message.data[3] = state->Trip2Consumption.data.leftByte;//Consumption
            message.data[4] = state->Trip2Consumption.data.rightByte;
            message.data[5] = state->Trip2LastResetSince.data.leftByte;
            message.data[6] = state->Trip2LastResetSince.data.rightByte;

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message) override
        {

        }
};
#endif
