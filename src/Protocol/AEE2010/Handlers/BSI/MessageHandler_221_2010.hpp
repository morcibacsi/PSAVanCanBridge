#pragma once

#ifndef _MessageHandler_221_2010_h
    #define _MessageHandler_221_2010_h

#include <cstdint>

#include "../../../IMessageHandler.hpp"
#include "../../Structs/CAN_221_2010.h"

class MessageHandler_221_2010 : public IMessageHandler<MessageHandler_221_2010>
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
        static constexpr uint32_t MessageId = 0x221;

        BusMessage Generate(CarState* carState)
        {
            CAN_221_2010_Byte1Struct byte1{};
            byte1.data.consumption_invalid       = carState->InvalidConsumption;
            byte1.data.remaining_range_invalid   = carState->InvalidRemainingRange;
            byte1.data.left_stick_button_pushed  = carState->LeftStickButtonPushed;
            byte1.data.right_stick_button_pushed = carState->RightStickButtonPushed;

            message.data[0] = byte1.asByte;
            message.data[1] = carState->InstantConsumption.data.leftByte;
            message.data[2] = carState->InstantConsumption.data.rightByte;
            message.data[3] = carState->RemainingRange.data.leftByte;
            message.data[4] = carState->RemainingRange.data.rightByte;
            message.data[5] = carState->TotalRange.data.leftByte;
            message.data[6] = carState->TotalRange.data.rightByte;

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {

        }
};
#endif
