#pragma once

#ifndef _MessageHandler_276_2010_h
    #define _MessageHandler_276_2010_h

#include <cstdint>

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

        BusMessage Generate(CarState* carState)
        {
            CAN_276_2010_Byte1Struct byte1{};
            byte1.data.year = carState->Year - 2000;
            byte1.data.time_format = carState->TIME_FORMAT_24H;

            CAN_276_2010_Byte2Struct byte2{};
            byte2.data.month = carState->Month;

            CAN_276_2010_Byte3Struct byte3{};
            byte3.data.day = carState->MDay;

            CAN_276_2010_Byte4Struct byte4{};
            byte4.data.hour = carState->Hour;

            CAN_276_2010_Byte5Struct byte5{};
            byte5.data.minute = carState->Minute;

            CAN_276_2010_BytesStruct urea_info{};
            urea_info.setUrea(carState->UreaRemaining.asUint16);
            urea_info.field.request_display = carState->ShowUreaRemaining;

            message.data[0] = byte1.asByte;
            message.data[1] = byte2.asByte;
            message.data[2] = byte3.asByte;
            message.data[3] = byte4.asByte;
            message.data[4] = byte5.asByte;
            message.data[5] = urea_info.asBytes[0];
            message.data[6] = urea_info.asBytes[1];
            message.data[7] = 0x00;

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {

        }
};
#endif
