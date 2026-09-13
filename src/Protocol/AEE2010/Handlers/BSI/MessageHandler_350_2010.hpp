#pragma once

#ifndef _MessageHandler_350_2010_h
    #define _MessageHandler_350_2010_h

#include <cstdint>
#include <cstring>

#include "../../Structs/CAN_350_2010.h"
#include "../../../IMessageHandler.hpp"

class MessageHandler_350_2010 : public IMessageHandler<MessageHandler_350_2010>
{
    private:
        BusMessage message
        {
            .id = MessageId,
            .data = {0},
            .dataLength = 8,
            .protocol = ProtocolType::AEE2010,
            .periodicityMs = 500-10,
            .offsetMs = 25,
            .isActive = true
        };

        uint8_t GetFanSpeedFromCarState(CarState* carState)
        {
            if (carState->AirConditionerStatus.data.FanSpeed == 0x0F)
            {
                return 1;
            }

            return carState->AirConditionerStatus.data.FanSpeed + 1;
        }
    public:
        static constexpr uint32_t MessageId = 0x350;

        BusMessage Generate(CarState* carState)
        {
            Can350_2010Byte1Struct byte1{};
            byte1.data.ac_type = 1; //Normal

            if (carState->AirConditionerStatus.data.IsHeatingPanelOn == 0)
            {
                byte1.data.ac_request = 0b10;
            }
            else
            {
                byte1.data.ac_request = 0;
            }

            Can350_2010Byte4Struct byte4{};
            byte4.data.mono_mode_active = carState->AirConMonoMode;
            byte4.data.front_left_temperature = carState->AirConLeftTemperature;

            Can350_2010Byte5Struct byte5{};
            byte5.data.front_right_temperature = carState->AirConRightTemperature;

            Can350_2010Byte6Struct byte6{};
            byte6.data.fan_speed = GetFanSpeedFromCarState(carState);

            Can350_2010Byte7Struct byte7{};
            byte7.data.front_left_distribution  = carState->AirConditionerStatus.data.Direction;
            byte7.data.front_right_distribution = carState->AirConditionerStatus.data.Direction;

            message.data[0] = byte1.asByte;
            message.data[1] = carState->AirConEvaporatorTemperature1;
            message.data[2] = carState->AirConEvaporatorTemperature2;
            message.data[3] = byte4.asByte;
            message.data[4] = byte5.asByte;
            message.data[5] = byte6.asByte;
            message.data[6] = byte7.asByte;
            message.data[7] = 0x00;

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {

        }
};
#endif
