#pragma once

#ifndef _MessageHandler_220_2004_h
    #define _MessageHandler_220_2004_h

#include <cstdint>
#include <cstring>

#include "../../Structs/CAN_220_2004.h"
#include "../../../IMessageHandler.hpp"
#include "../../../ImmediateSignal.hpp"

class MessageHandler_220 : public IMessageHandler<MessageHandler_220>
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
        static constexpr uint32_t MessageId = 0x220;

        MessageHandler_220()
        {
        }

        void SetImmediateSignalCallback(ImmediateSignalCallback immediateSignalCallback) { _immediateSignalCallback = immediateSignalCallback; }

        BusMessage Generate(CarState* carState)
        {
            CAN_220_2004_Byte1Struct field1{};
            field1.data.front_left_door_open  = carState->DoorStatus.data.front_left_door_open;
            field1.data.front_right_door_open = carState->DoorStatus.data.front_right_door_open;

            field1.data.rear_left_door_open   = carState->DoorStatus.data.rear_left_door_open;
            field1.data.rear_right_door_open  = carState->DoorStatus.data.rear_right_door_open;

            field1.data.hood_open  = carState->DoorStatus.data.hood_open;
            field1.data.trunk_open = carState->DoorStatus.data.trunk_open;

            field1.data.fuel_flap_open   = carState->DoorStatus.data.fuel_flap_open;
            field1.data.rear_window_open = carState->DoorStatus.data.rear_window_open;

            message.data[0] = field1.asByte;
            message.data[1] = 0x00;

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {
            Can220_2004_Struct packet;
            std::memcpy(&packet, message.data, sizeof(packet));

            carState->DoorStatus.data.front_left_door_open  = packet.Field1.data.front_left_door_open;
            carState->DoorStatus.data.front_right_door_open = packet.Field1.data.front_right_door_open;
            carState->DoorStatus.data.rear_left_door_open   = packet.Field1.data.rear_left_door_open;
            carState->DoorStatus.data.rear_right_door_open  = packet.Field1.data.rear_right_door_open;
            carState->DoorStatus.data.trunk_open            = packet.Field1.data.trunk_open;
            carState->DoorStatus.data.hood_open             = packet.Field1.data.hood_open;
            carState->DoorStatus.data.fuel_flap_open        = packet.Field1.data.fuel_flap_open;
            carState->DoorStatus.data.rear_window_open      = packet.Field1.data.rear_window_open;

            if (_immediateSignalCallback)
            {
                _immediateSignalCallback(ImmediateSignal::PopupMessage);
            }
        }
};
#endif
