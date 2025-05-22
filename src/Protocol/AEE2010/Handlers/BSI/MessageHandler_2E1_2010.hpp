#pragma once

#ifndef _MessageHandler_2E1_2010_h
    #define _MessageHandler_2E1_2010_h

#include <cstdint>

#include "../../../IMessageHandler.hpp"
#include "../../Structs/CAN_2E1_2010.h"

class MessageHandler_2E1_2010 : public IMessageHandler<MessageHandler_2E1_2010>
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
        static constexpr uint32_t MessageId = 0x2E1;

        BusMessage Generate(CarState* carState)
        {
            CAN_2E1_2010_Byte1Struct field1{};
            field1.data.automatic_door_locking_status = carState->DeadlockActive ? AUTO_DOOR_LOCKING_STATUS_ACTIVATED_2010 : AUTO_DOOR_LOCKING_STATUS_NOT_ACTIVATED_2010;
            field1.data.auto_headlight_status = carState->AutoHeadlampActive ? AUTO_HEADLIGHT_STATUS_ACTIVATED_2010 : AUTO_HEADLIGHT_STATUS_NOT_ACTIVATED_2010;
            field1.data.passenger_airbag_status = carState->PassengerAirbag ? PASSENGER_AIRBAG_STATUS_NOT_ACTIVATED_2010 : PASSENGER_AIRBAG_STATUS_ACTIVATED_2010;

            CAN_2E1_2010_Byte2Struct field2{};
            field2.data.auto_wiper_status = carState->AutoWipingActive ? AUTO_WIPER_STATUS_ACTIVATED_2010 : AUTO_WIPER_STATUS_NOT_ACTIVATED_2010;
            field2.data.esp_status = carState->EspDeactivated ? ESP_STATUS_NOT_ACTIVATED_2010 : ESP_STATUS_ACTIVATED_2010;

            CAN_2E1_2010_Byte3Struct field3{};
            field3.data.child_safety_status = carState->PassengerAirbag ? CHILD_SAFETY_STATUS_ACTIVATED_2010 : CHILD_SAFETY_STATUS_NOT_ACTIVATED_2010;

            message.data[0] = field1.asByte;
            message.data[1] = field2.asByte;
            message.data[2] = field3.asByte;
            message.data[3] = 0x00;
            message.data[4] = 0x00;
            message.data[5] = 0x00;

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {

        }
};
#endif
