#pragma once

#ifndef _MessageHandler_276_2010_h
    #define _MessageHandler_276_2010_h

#include <inttypes.h>
#include <memory>
#include "../../../IMessageHandler.hpp"
#include "../../Structs/CAN_276_2010.h"

class MessageHandler_276_2010 : public IMessageHandler<MessageHandler_276_2010>
{
    private:
        BusMessage message
        {
            .id = 0x276,
            .command = 0,
            .data = {0},
            .dataLength = 8,
            .protocol = ProtocolType::AEE2010,
            .periodicityMs = 1000,
            .offsetMs = 130,
            .isActive = true
        };
    public:
        static constexpr uint32_t MessageId = 0x276;

        BusMessage Generate(CarState* state)
        {
            CAN_276_2010_Byte1Struct byte1{};
            byte1.data.year = state->Year - 2000;
            byte1.data.time_format = 1; // 24 hour format

            CAN_276_2010_Byte2Struct byte2{};
            byte2.data.month = state->Month;

            CAN_276_2010_Byte3Struct byte3{};
            byte3.data.day = state->MDay;

            CAN_276_2010_Byte4Struct byte4{};
            byte4.data.hour = state->Hour;

            CAN_276_2010_Byte5Struct byte5{};
            byte5.data.minute = state->Minute;

            message.data[0] = byte1.asByte;
            message.data[1] = byte2.asByte;
            message.data[2] = byte3.asByte;
            message.data[3] = byte4.asByte;
            message.data[4] = byte5.asByte;
            message.data[5] = 0x00;
            message.data[6] = 0x00;
            message.data[7] = 0x00;

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {

        }
};
#endif
