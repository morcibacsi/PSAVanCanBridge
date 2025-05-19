#pragma once

#ifndef _MessageHandler_1A8_2010_h
    #define _MessageHandler_1A8_2010_h

#include <inttypes.h>
#include <memory>
#include "../../../IMessageHandler.hpp"

class MessageHandler_1A8_2010 : public IMessageHandler<MessageHandler_1A8_2010>
{
    private:
        BusMessage message
        {
            .id = 0x1A8,
            .data = {0},
            .dataLength = 8,
            .protocol = ProtocolType::AEE2010,
            .periodicityMs = 200,
            .offsetMs = 90,
            .isActive = true
        };
    public:
        static constexpr uint32_t MessageId = 0x1A8;

        BusMessage Generate(CarState* state)
        {
            uint8_t field1 = 0x00;
            if (state->CruiseControlActivateFunction)
            {
                field1 = 0x02;
            } else
            {
                field1 = 0x00;
            }

            message.data[0] = field1;
            message.data[1] = state->CruiseControlSpeed.data.leftByte;
            message.data[2] = state->CruiseControlSpeed.data.rightByte;
            message.data[3] = state->CruiseControlSpeed.data.leftByte;
            message.data[4] = state->CruiseControlSpeed.data.rightByte;
            message.data[5] = state->TripOnCMB.data.leftByte;
            message.data[6] = state->TripOnCMB.data.middleByte;
            message.data[7] = state->TripOnCMB.data.rightByte;

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {

        }
};
#endif
