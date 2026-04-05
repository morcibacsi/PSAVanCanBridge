#pragma once

#ifndef _MessageHandler_136_h
    #define _MessageHandler_136_h

#include <cstdint>
#include <cstring>

#include "../../../IMessageHandler.hpp"
#include "../../Structs/CAN_136_2004.h"

class MessageHandler_136 : public IMessageHandler<MessageHandler_136>
{
    private:
        BusMessage message
        {
            .id = 0x136,
            .data = { 0 },
            .dataLength = 3,
            .protocol = ProtocolType::AEE2004,
            .periodicityMs = 500,
            .offsetMs = 130,
            .isActive = false
        };
    public:
        static constexpr uint32_t MessageId = 0x136;

        BusMessage Generate(CarState* carState)
        {
            return message;
        }

        void Parse(CarState* carState, const BusMessage& msg)
        {
            CAN_136_2004_BytesStruct packet;
            std::memcpy(&packet, message.data, sizeof(packet));

            carState->UreaRemaining.asUint16 = packet.field.urea_remaining;
            carState->ShowUreaRemaining      = packet.field.request_urea_display;
        }
};
#endif
