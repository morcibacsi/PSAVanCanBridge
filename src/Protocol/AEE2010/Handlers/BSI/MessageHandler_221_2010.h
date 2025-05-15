#pragma once

#ifndef _MessageHandler_221_2010_h
    #define _MessageHandler_221_2010_h

#include <inttypes.h>
#include <memory>
#include "../../../IMessageHandler.hpp"
#include "../../Structs/CAN_221_2010.h"

class MessageHandler_221_2010 : public IMessageHandler
{
    private:
        BusMessage message
        {
            .id = 0x221,
            .data = {0},
            .dataLength = 7,
            .protocol = ProtocolType::AEE2010,
            .periodicityMs = 1000-150,
            .offsetMs = 990,
            .isActive = true
        };
    public:
        BusMessage Generate(CarState* state) override
        {
            CAN_221_2010_Byte1Struct byte1{};
            byte1.data.consumption_invalid       = state->InvalidConsumption;
            byte1.data.remaining_range_invalid   = state->InvalidRemainingRange;
            byte1.data.left_stick_button_pushed  = state->LeftStickButtonPushed;
            byte1.data.right_stick_button_pushed = state->RightStickButtonPushed;

            message.data[0] = byte1.asByte;
            message.data[1] = state->InstantConsumption.data.leftByte;
            message.data[2] = state->InstantConsumption.data.rightByte;
            message.data[3] = state->RemainingRange.data.leftByte;
            message.data[4] = state->RemainingRange.data.rightByte;
            message.data[5] = state->TotalRange.data.leftByte;
            message.data[6] = state->TotalRange.data.rightByte;

            return message;
        }
};
#endif
