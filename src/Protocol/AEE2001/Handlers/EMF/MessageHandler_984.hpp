#pragma once

#ifndef _MessageHandler_984_h
    #define _MessageHandler_984_h

#include <cstdint>
#include <cstring>

#include "../../../IMessageHandler.hpp"

class MessageHandler_984 : public IMessageHandler<MessageHandler_984>
{
    private:
        BusMessage message
        {
            .id = MessageId,
            .data = {0},
            .dataLength = 5,
            .ack = false,
            .protocol = ProtocolType::AEE2001,
            .type = MessageType::Normal,
            .periodicityMs = 60000,
            .offsetMs = 0,
            .isActive = false
        };

    public:
        static constexpr uint32_t MessageId = 0x984;

        MessageHandler_984(
        )
        {
        }

        BusMessage Generate(CarState* carState)
        {
            bool emulateDisplay = carState->EMULATE_DISPLAY_ON_SOURCE;
            bool reverseNotEngaged = (carState->IsReverseEngaged == 0);
            bool reverseEngagedButParkingAidIsNotVanBusType =
                (carState->IsReverseEngaged == 1 && carState->PARKING_AID_TYPE != 0x01);

            message.isActive = emulateDisplay && (reverseNotEngaged || reverseEngagedButParkingAidIsNotVanBusType);

            message.data[0] = 0x00;
            message.data[1] = 0x00;
            message.data[2] = 0x00;
            message.data[3] = 0xFF;
            message.data[4] = 0xFF;

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {

        }
};
#endif
