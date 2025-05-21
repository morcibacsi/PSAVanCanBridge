#pragma once

#ifndef _MessageHandler_221_2004_h
    #define _MessageHandler_221_2004_h

#include <inttypes.h>
#include <memory>
#include <cstring>

#include "../../Structs/CAN_221_2004.h"
#include "../../../IMessageHandler.hpp"

class MessageHandler_221 : public IMessageHandler<MessageHandler_221>
{
    private:
        BusMessage message
        {
            .id = 0x221,
            .command = 0,
            .data = {0},
            .dataLength = 7,
            .protocol = ProtocolType::AEE2004,
            .periodicityMs = 1000-250,
            .offsetMs = 25,
            .isActive = true
        };

        ImmediateSignalCallback _immediateSignalCallback;

    public:
        static constexpr uint32_t MessageId = 0x221;

        MessageHandler_221()
        {
        }

        void SetImmediateSignalCallback(ImmediateSignalCallback immediateSignalCallback) { _immediateSignalCallback = immediateSignalCallback; }

        BusMessage Generate(CarState* carState)
        {
            CAN_221_2004_Byte1Struct field1{};
            field1.data.left_stick_button_pushed  = carState->LeftStickButtonPushed;
            field1.data.right_stick_button_pushed = carState->RightStickButtonPushed;
            //printf("MessageHandler_221::Generate - RightStickButtonPushed: %d\n", carState->RightStickButtonPushed);
            field1.data.remaining_range_invalid   = carState->InvalidRemainingRange;
            field1.data.consumption_invalid       = carState->InvalidConsumption;

            message.data[0] = field1.asByte;

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
            CAN_221_2004_Struct packet;
            std::memcpy(&packet, message.data, sizeof(packet));

            carState->RightStickButtonPushed = packet.Field1.data.right_stick_button_pushed;
            carState->LeftStickButtonPushed  = packet.Field1.data.left_stick_button_pushed;

            carState->InvalidRemainingRange  = packet.Field1.data.remaining_range_invalid;
            carState->InvalidConsumption     = packet.Field1.data.consumption_invalid;

            carState->InstantConsumption.data.leftByte  = packet.InstantConsumptionByte1;
            carState->InstantConsumption.data.rightByte = packet.InstantConsumptionByte2;

            carState->RemainingRange.data.leftByte  = packet.RemainingRangeByte1;
            carState->RemainingRange.data.rightByte = packet.RemainingRangeByte2;

            carState->TotalRange.data.leftByte      = packet.TotalRangeByte1;
            carState->TotalRange.data.rightByte     = packet.TotalRangeByte1;

            if (_immediateSignalCallback)
            {
                _immediateSignalCallback(ImmediateSignal::TripButtonPressed);
            }
        }
};
#endif
