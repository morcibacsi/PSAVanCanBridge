#pragma once

#ifndef _MessageHandler_1A8_h
    #define _MessageHandler_1A8_h

#include <inttypes.h>
#include <memory>
#include <cstring>

#include "../../Structs/CAN_1A8_2004.h"
#include "../../../IMessageHandler.hpp"

class MessageHandler_1A8 : public IMessageHandler
{
    private:
        BusMessage message
        {
            .id = 0x1A8,
            .data = { 0 },
            .dataLength = 8,
            .protocol = ProtocolType::AEE2004,
            .periodicityMs = 200,
            .offsetMs = 90,
            .isActive = true
        };

        std::function<void(ImmediateSignal)> _immediateSignalCallback;

    public:
        MessageHandler_1A8(
            std::function<void(ImmediateSignal)> immediateSignalCallback
        )
        {
            _immediateSignalCallback = immediateSignalCallback;
        }

        BusMessage Generate(std::shared_ptr<CarState> state) override
        {
            CAN_1A8_2004_Byte1Struct field1{};
            field1.data.unit_of_speed               = state->CruiseControlSpeedUnit;
            field1.data.activate_function           = state->CruiseControlActivateFunction;
            field1.data.selected_function           = state->CruiseControlSelectedFunction;
            field1.data.status_of_selected_function = state->CruiseControlStatusOfSelectedFunction;

            message.data[0] = field1.asByte;
            message.data[1] = state->CruiseControlSpeed.data.leftByte;
            message.data[2] = state->CruiseControlSpeed.data.rightByte;
            message.data[3] = 0x00;
            message.data[4] = 0x00;
            message.data[5] = state->TripOnCMB.data.leftByte;
            message.data[6] = state->TripOnCMB.data.middleByte;
            message.data[7] = state->TripOnCMB.data.rightByte;

            return message;
        }

        void Parse(std::shared_ptr<CarState> carState, const BusMessage& message) override
        {
            //CAN_1A8_2004Struct tmp;
            //std::memcpy(&tmp, message.data, static_cast<std::size_t>(sizeof(tmp)));
            const auto* tmp = reinterpret_cast<const CAN_1A8_2004Struct*>(message.data);

            carState->CruiseControlSpeed.data.leftByte      = tmp->CruiseControlSpeed1;
            carState->CruiseControlSpeed.data.rightByte     = tmp->CruiseControlSpeed2;
            carState->CruiseControlActivateFunction         = tmp->Field1.data.activate_function;
            carState->CruiseControlSelectedFunction         = tmp->Field1.data.selected_function;
            carState->CruiseControlStatusOfSelectedFunction = tmp->Field1.data.status_of_selected_function;
            carState->TripOnCMB.data.leftByte               = tmp->TripOnCMB1;
            carState->TripOnCMB.data.middleByte             = tmp->TripOnCMB2;
            carState->TripOnCMB.data.rightByte              = tmp->TripOnCMB3;

            if (_immediateSignalCallback)
            {
                _immediateSignalCallback(ImmediateSignal::CruiseControl);
            }
        }
};
#endif
