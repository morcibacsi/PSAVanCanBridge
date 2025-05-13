#pragma once

#ifndef _MessageHandler_1A1_2010_h
    #define _MessageHandler_1A1_2010_h

#include <inttypes.h>
#include <memory>
#include "../../../IMessageHandler.hpp"
#include "../../Structs/CanDisplayStructs.h"

class MessageHandler_1A1_2010 : public IMessageHandler
{
    private:
        uint8_t _prevDoorStatus;

        BusMessage message
        {
            .id = 0x1A1,
            .data = {0},
            .dataLength = 8,
            .protocol = ProtocolType::AEE2010,
            .periodicityMs = 200,
            .offsetMs = 110,
            .isActive = true
        };

    public:
        BusMessage Generate(std::shared_ptr<CarState> state) override
        {
            message.data[0] = state->DisplayMessage.data.Field1;
            message.data[1] = state->DisplayMessage.data.Field2;
            message.data[2] = state->DisplayMessage.data.Field3;
            message.data[3] = state->DisplayMessage.data.Field4;
            message.data[4] = state->DisplayMessage.data.Field5;
            message.data[5] = state->DisplayMessage.data.Field6;
            message.data[6] = state->DisplayMessage.data.Field7;
            message.data[7] = state->DisplayMessage.data.Field8;

            bool isDoorOpen = state->DoorStatus.asByte > 0;
            bool doorStateChanged = _prevDoorStatus != state->DoorStatus.asByte;
            bool showDoorMessage =
                state->Ignition &&
                state->GENERATE_POPUP_FOR_DOOR_STATUS &&
                (state->DisplayMessage.data.Field1 == 0xFF || state->DisplayMessage.data.Field2 == 0xFF) &&
                (isDoorOpen || doorStateChanged);

            if (showDoorMessage == false)
            {
                return message;
            }

            if (doorStateChanged)
            {
                message.data[0] = CAN_POPUP_MSG_HIDE;
                message.data[1] = CAN_POPUP_MSG_CLEAR;
                _prevDoorStatus = state->DoorStatus.asByte;
                return message;
            }

            if (isDoorOpen)
            {
                CanDisplayByte2Struct field2;
                field2.data.priority = 1;
                field2.data.show_popup_on_cmb = 1;
                field2.data.show_popup_on_emf = 1;
                field2.data.show_popup_on_vth = 1;

                CanDisplayDoorStatus1Struct doorStatus;
                doorStatus.asByte = 0;
                doorStatus.data.is_front_left_open  = state->DoorStatus.data.front_left_door_open;
                doorStatus.data.is_front_right_open = state->DoorStatus.data.front_right_door_open;
                doorStatus.data.is_rear_left_open   = state->DoorStatus.data.rear_left_door_open;
                doorStatus.data.is_rear_right_open  = state->DoorStatus.data.rear_right_door_open;
                doorStatus.data.is_boot_open        = state->DoorStatus.data.trunk_open;
                doorStatus.data.is_bonnet_open      = state->DoorStatus.data.hood_open;

                CanDisplayDoorStatus2Struct doorStatus2;
                doorStatus2.asByte = 0;
                doorStatus2.data.is_fuel_flap_open = state->DoorStatus.data.fuel_flap_open;

                message.data[0] = CAN_POPUP_MSG_SHOW_CATEGORY1;
                message.data[1] = CAN_POPUP_MSG_DOORS_BOOT_BONNET_REAR_SCREEN_AND_FUEL_TANK_OPEN;
                message.data[2] = field2.asByte;
                message.data[3] = doorStatus.asByte;
                message.data[4] = doorStatus2.asByte;
            }

            return message;
        }
};
#endif
