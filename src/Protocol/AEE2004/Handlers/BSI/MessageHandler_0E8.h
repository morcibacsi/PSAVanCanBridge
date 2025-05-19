#pragma once

#ifndef _MessageHandler_0E8_h
    #define _MessageHandler_0E8_h

#include <inttypes.h>
#include <memory>
#include <cstring>

#include "../../Structs/CAN_0E8.h"
#include "../../../IMessageHandler.hpp"

class MessageHandler_0E8 : public IMessageHandler<MessageHandler_0E8>
{
    private:
        BusMessage message
        {
            .id = 0x0E8,
            .data = { 0 },
            .dataLength = 7,
            .protocol = ProtocolType::AEE2004,
            .periodicityMs = 200,
            .offsetMs = 60,
            .isActive = true
        };
    public:
        static constexpr uint32_t MessageId = 0x0E8;

        BusMessage Generate(CarState* state)
        {
            CAN_0E8_Byte1Struct field1{};
            field1.data.key_alert = 0;//TODO check on VAN
            field1.data.hood_open             = state->DoorStatus.data.hood_open;
            field1.data.rear_left_door_open   = state->DoorStatus.data.rear_left_door_open;
            field1.data.rear_right_door_open  = state->DoorStatus.data.rear_right_door_open;
            field1.data.front_left_door_open  = state->DoorStatus.data.front_left_door_open;
            field1.data.front_right_door_open = state->DoorStatus.data.front_right_door_open;

            bool isDoorOpen =
                state->DoorStatus.data.front_left_door_open ||
                state->DoorStatus.data.front_right_door_open ||
                state->DoorStatus.data.rear_left_door_open ||
                state->DoorStatus.data.rear_right_door_open;

            if (state->SpeedInKmh < 10 && isDoorOpen)
            {
                field1.data.door_alert_below_10kmph = 1;
            }
            else if (state->SpeedInKmh >= 10 && isDoorOpen)
            {
                field1.data.door_alert_above_10kmph = 1;
            }

            CAN_0E8_Byte2Struct field2{};
            field2.data.rear_window_open = state->DoorStatus.data.rear_window_open;
            field2.data.trunk_open       = state->DoorStatus.data.trunk_open;
            field2.data.vehicle_type     = 0;

            if (state->SpeedInKmh < 10 && state->DoorStatus.data.fuel_flap_open)
            {
                field2.data.fuel_flap_open_below_10kmph = 1;

            }
            else if (state->SpeedInKmh >= 10 && state->DoorStatus.data.fuel_flap_open)
            {
                field2.data.fuel_flap_open_above_10kmph = 1;
            }
            if (state->SpeedInKmh > 1)
            {
                field2.data.handbrake_on_while_accelerating = 1;
            }

            CAN_0E8_Byte5Struct field5{};
            field5.data.auto_lighting_alert_enabled = state->State_AutoLightsEnabled;
            field5.data.auto_lighting_alert_disabled = !state->State_AutoLightsEnabled;
            field5.data.auto_wiper_alert_enabled = state->State_AutoWipeEnabled;
            field5.data.auto_wiper_alert_disabled = !state->State_AutoWipeEnabled;
            field5.data.lock_alert_while_driving_enabled = state->State_AutoLockEnabled;
            field5.data.lock_alert_while_driving_disabled = !state->State_AutoLockEnabled;

            message.data[0] = field1.asByte;
            message.data[1] = field2.asByte;
            message.data[2] = 0x00;
            message.data[3] = 0x00;
            message.data[4] = 0x00;
            message.data[5] = field5.asByte;

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {
            //Can0E6Struct tmp;
            //std::memcpy(&tmp, message.data, static_cast<std::size_t>(sizeof(tmp)));
            const auto* tmp = reinterpret_cast<const Can0E8Struct*>(message.data);
        }
};
#endif
