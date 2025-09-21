#pragma once

#ifndef _MessageHandler_126_h
    #define _MessageHandler_126_h

#include <cstdint>

#include "../../../IMessageHandler.hpp"
#include "../../Structs/CAN_126_2004.h"

class MessageHandler_126 : public IMessageHandler<MessageHandler_126>
{
    private:
        BusMessage message
        {
            .id = 0x126,
            .data = { 0 },
            .dataLength = 3,
            .protocol = ProtocolType::AEE2004,
            .periodicityMs = 100,
            .offsetMs = 440,
            .isActive = true
        };
    public:
        static constexpr uint32_t MessageId = 0x126;

        BusMessage Generate(CarState* carState)
        {
            message.isActive = carState->SOURCE_PROTOCOL == static_cast<uint8_t>(ProtocolType::AEE2001) && carState->PARKING_AID_TYPE == 0;

            CAN_126_2004_Byte1Struct field1{};

            CAN_126_2004_Byte2Struct field2{};
            field2.data.gearbox_selection = carState->GearBoxSelection;

            message.data[0] = field1.asByte;
            message.data[1] = field2.asByte;
            message.data[2] = 0x00;

            return message;
        }

        void Parse(CarState* carState, const BusMessage& msg)
        {
        }
};
#endif
