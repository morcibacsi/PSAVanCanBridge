#pragma once

#ifndef _MessageHandler_236_2010_h
    #define _MessageHandler_236_2010_h

#include <cstdint>

#include "../../../IMessageHandler.hpp"
#include "../../Structs/CAN_236_2010.h"

class MessageHandler_236_2010 : public IMessageHandler<MessageHandler_236_2010>
{
    private:
        BusMessage message
        {
            .id = 0x236,
            .data = { 0 },
            .dataLength = 8,
            .protocol = ProtocolType::AEE2010,
            .periodicityMs = 100,
            .offsetMs = 790,
            .isActive = true
        };

    public:
        static constexpr uint32_t MessageId = 0x236;

        BusMessage Generate(CarState* carState)
        {
            CAN_236_2010_Byte1Struct byte1{};
            byte1.data.vehicle_config = CAN_2010_VEHICLE_CONFIG_CLIENT;
            byte1.data.electric_network_status = CAN_2010_ELECTRIC_NETWORK_STATUS_NORMAL;

            if (carState->EconomyMode)
            {
                byte1.data.electric_network_status = CAN_2010_ELECTRIC_NETWORK_STATUS_ECO;
            }

            CAN_236_2010_Byte6Struct byte6{};
            byte6.data.trunk_status = carState->DoorStatus.data.trunk_open;
            byte6.data.driver_door_status = carState->DoorStatus.data.front_left_door_open;
            //byte6.data.brake_pedal_status = carState->FootOnBrake == 0 ? 1 : 0;

            CAN_236_2010_Byte8Struct byte8{};
            byte8.data.passenger_door_status = carState->DoorStatus.data.front_right_door_open;
            byte8.data.rear_left_door_status = carState->DoorStatus.data.rear_left_door_open;
            byte8.data.rear_right_door_status = carState->DoorStatus.data.rear_right_door_open;
            byte8.data.driver_door_status_estimated = carState->DoorStatus.data.front_left_door_open;

            message.data[0] = byte1.asByte;
            message.data[1] = 0x03;
            message.data[2] = 0xDE;
            message.data[3] = 0x00;
            message.data[4] = 0x00;
            message.data[5] = byte6.asByte;
            message.data[6] = 0xFE;
            message.data[7] = byte8.asByte;

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {

        }
};
#endif
