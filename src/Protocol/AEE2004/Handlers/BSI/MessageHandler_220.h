#pragma once

#ifndef _MessageHandler_220_2004_h
    #define _MessageHandler_220_2004_h

#include <inttypes.h>
#include <memory>
#include <cstring>

#include "../../Structs/CAN_220.h"
#include "../../../IMessageHandler.hpp"

class MessageHandler_220 : public IMessageHandler
{
    private:
        BusMessage message
        {
            .id = 0x220,
            .data = { 0 },
            .dataLength = 2,
            .protocol = ProtocolType::AEE2004,
            .periodicityMs = 500,
            .offsetMs = 25,
            .isActive = true
        };

        ImmediateSignalCallback _immediateSignalCallback;

    public:
        MessageHandler_220(
            ImmediateSignalCallback immediateSignalCallback
        )
        {
            _immediateSignalCallback = immediateSignalCallback;
        }

        BusMessage Generate(CarState* state) override
        {
            CAN_220_2004_Byte1Struct field1{};
            field1.data.front_left_door_open  = state->DoorStatus.data.front_left_door_open;
            field1.data.front_right_door_open = state->DoorStatus.data.front_right_door_open;

            field1.data.rear_left_door_open   = state->DoorStatus.data.rear_left_door_open;
            field1.data.rear_right_door_open  = state->DoorStatus.data.rear_right_door_open;

            field1.data.hood_open  = state->DoorStatus.data.hood_open;
            field1.data.trunk_open = state->DoorStatus.data.trunk_open;

            field1.data.fuel_flap_open   = state->DoorStatus.data.fuel_flap_open;
            field1.data.rear_window_open = state->DoorStatus.data.rear_window_open;

            message.data[0] = field1.asByte;
            message.data[1] = 0x00;

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message) override
        {
            //Can220_2004_Struct tmp;
            //std::memcpy(&tmp, message.data, static_cast<std::size_t>(sizeof(tmp)));
            const auto* tmp = reinterpret_cast<const Can220_2004_Struct*>(message.data);

            carState->DoorStatus.data.front_left_door_open  = tmp->Field1.data.front_left_door_open;
            carState->DoorStatus.data.front_right_door_open = tmp->Field1.data.front_right_door_open;
            carState->DoorStatus.data.rear_left_door_open   = tmp->Field1.data.rear_left_door_open;
            carState->DoorStatus.data.rear_right_door_open  = tmp->Field1.data.rear_right_door_open;
            carState->DoorStatus.data.trunk_open            = tmp->Field1.data.trunk_open;
            carState->DoorStatus.data.hood_open             = tmp->Field1.data.hood_open;
            carState->DoorStatus.data.fuel_flap_open        = tmp->Field1.data.fuel_flap_open;
            carState->DoorStatus.data.rear_window_open      = tmp->Field1.data.rear_window_open;

            if (_immediateSignalCallback)
            {
                _immediateSignalCallback(ImmediateSignal::PopupMessage);
            }
        }
};
#endif
