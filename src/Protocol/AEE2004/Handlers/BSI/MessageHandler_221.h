#pragma once

#ifndef _MessageHandler_221_2004_h
    #define _MessageHandler_221_2004_h

#include <inttypes.h>
#include <memory>
#include <cstring>

#include "../../Structs/CAN_221_2004.h"
#include "../../../IMessageHandler.hpp"

class MessageHandler_221 : public IMessageHandler
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
        MessageHandler_221(
            ImmediateSignalCallback immediateSignalCallback
        )
        {
            _immediateSignalCallback = immediateSignalCallback;
        }

        BusMessage Generate(CarState* state) override
        {
            CAN_221_2004_Byte1Struct field1{};
            field1.data.left_stick_button_pushed  = state->LeftStickButtonPushed;
            field1.data.right_stick_button_pushed = state->RightStickButtonPushed;
            //printf("MessageHandler_221::Generate - RightStickButtonPushed: %d\n", state->RightStickButtonPushed);
            field1.data.remaining_range_invalid   = state->InvalidRemainingRange;
            field1.data.consumption_invalid       = state->InvalidConsumption;

            message.data[0] = field1.asByte;

            message.data[1] = state->InstantConsumption.data.leftByte;
            message.data[2] = state->InstantConsumption.data.rightByte;

            message.data[3] = state->RemainingRange.data.leftByte;
            message.data[4] = state->RemainingRange.data.rightByte;

            message.data[5] = state->TotalRange.data.leftByte;
            message.data[6] = state->TotalRange.data.rightByte;

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message) override
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
