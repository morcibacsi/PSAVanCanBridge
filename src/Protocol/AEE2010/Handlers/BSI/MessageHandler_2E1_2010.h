#pragma once

#ifndef _MessageHandler_2E1_2010_h
    #define _MessageHandler_2E1_2010_h

#include <inttypes.h>
#include <memory>
#include "../../../IMessageHandler.hpp"
#include "../../Structs/CAN_2E1_2010.h"

class MessageHandler_2E1_2010 : public IMessageHandler
{
    private:
        BusMessage message
        {
            .id = 0x2E1,
            .data = {0},
            .dataLength = 6,
            .protocol = ProtocolType::AEE2010,
            .periodicityMs = 1000,
            .offsetMs = 580,
            .isActive = true
        };
    public:
        BusMessage Generate(std::shared_ptr<CarState> state) override
        {
            CAN_2E1_2010_Byte1Struct field1{};
            field1.data.automatic_door_locking_status = state->DeadlockActive ? AUTO_DOOR_LOCKING_STATUS_ACTIVATED : AUTO_DOOR_LOCKING_STATUS_NOT_ACTIVATED;
            field1.data.auto_headlight_status = state->AutoHeadlampActive ? AUTO_HEADLIGHT_STATUS_ACTIVATED : AUTO_HEADLIGHT_STATUS_NOT_ACTIVATED;
            field1.data.passenger_airbag_status = state->PassengerAirbag ? PASSENGER_AIRBAG_STATUS_NOT_ACTIVATED : PASSENGER_AIRBAG_STATUS_ACTIVATED;

            CAN_2E1_2010_Byte2Struct field2{};
            field2.data.auto_wiper_status = state->AutoWipingActive ? AUTO_WIPER_STATUS_ACTIVATED : AUTO_WIPER_STATUS_NOT_ACTIVATED;
            field2.data.esp_status = state->EspDeactivated ? ESP_STATUS_NOT_ACTIVATED : ESP_STATUS_ACTIVATED;

            CAN_2E1_2010_Byte3Struct field3{};
            field3.data.child_safety_status = state->PassengerAirbag ? CHILD_SAFETY_STATUS_ACTIVATED : CHILD_SAFETY_STATUS_NOT_ACTIVATED;

            message.data[0] = field1.asByte;
            message.data[1] = field2.asByte;
            message.data[2] = field3.asByte;
            message.data[3] = 0x00;
            message.data[4] = 0x00;
            message.data[5] = 0x00;

            return message;
        }
};
#endif
