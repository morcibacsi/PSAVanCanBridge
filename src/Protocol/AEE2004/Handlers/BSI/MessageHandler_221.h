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
            //CAN_221_2004_Struct tmp;
            //std::memcpy(&tmp, message.data, static_cast<std::size_t>(sizeof(tmp)));
            const auto* tmp = reinterpret_cast<const CAN_221_2004_Struct*>(message.data);

            carState->RightStickButtonPushed = tmp->Field1.data.right_stick_button_pushed;
            carState->LeftStickButtonPushed  = tmp->Field1.data.left_stick_button_pushed;

            carState->InvalidRemainingRange  = tmp->Field1.data.remaining_range_invalid;
            carState->InvalidConsumption     = tmp->Field1.data.consumption_invalid;

            carState->InstantConsumption.data.leftByte  = tmp->InstantConsumptionByte1;
            carState->InstantConsumption.data.rightByte = tmp->InstantConsumptionByte2;

            carState->RemainingRange.data.leftByte  = tmp->RemainingRangeByte1;
            carState->RemainingRange.data.rightByte = tmp->RemainingRangeByte2;

            carState->TotalRange.data.leftByte      = tmp->TotalRangeByte1;
            carState->TotalRange.data.rightByte     = tmp->TotalRangeByte1;

            if (_immediateSignalCallback)
            {
                _immediateSignalCallback(ImmediateSignal::TripButtonPressed);
            }
        }
};
#endif
