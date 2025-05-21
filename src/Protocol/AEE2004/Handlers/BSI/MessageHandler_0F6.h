#pragma once

#ifndef _MessageHandler_0F6_h
    #define _MessageHandler_0F6_h

#include <inttypes.h>
#include <memory>
#include <cstring>

#include "../../Structs/CAN_0F6.h"
#include "../../../IMessageHandler.hpp"

class MessageHandler_0F6 : public IMessageHandler<MessageHandler_0F6>
{
    private:
        BusMessage message
        {
            .id = 0x0F6,
            .data = {0},
            .dataLength = 8,
            .protocol = ProtocolType::AEE2004,
            .periodicityMs = 500,
            .offsetMs = 240,
            .isActive = true
        };
    public:
        static constexpr uint32_t MessageId = 0x0F6;

        BusMessage Generate(CarState* carState)
        {
            CanDash1Byte1Struct field1{};
            field1.data.key_position = KEY_POSITION_2004_CONTACT;
            if (carState->USE_IGNITION_SIGNAL_FROM_SOURCE_BUS)
            {
                field1.data.key_position = carState->KeyPosition;
            }

            field1.data.engine_status    = carState->EngineStatus;
            field1.data.generator_status = carState->EngineRunning;
            field1.data.factory_mode     = carState->FactoryMode;
            field1.data.config_mode      = carState->FactoryMode == 1 ? CONFIG_MODE_2004_FACTORY : CONFIG_MODE_2004_CLIENT;

            CanDash1Byte8Struct field8{};
            field8.data.reverse_gear_light = carState->IsReverseEngaged;
            field8.data.wiper_status       = carState->WiperStatus;
            field8.data.turn_left_light    = carState->CarSignalLights.data.left_turn_indicator;
            field8.data.turn_right_light   = carState->CarSignalLights.data.right_turn_indicator;

            //0x8E, 0x71, 0x21, 0x8D, 0xF3, 0x65, temperature, 0x50
/*
            message.data.push_back(0x8E);
            message.data.push_back(0x71);
            message.data.push_back(0x21);
            message.data.push_back(0x8D);
            message.data.push_back(0xF3);
            message.data.push_back(0x65);
            message.data.push_back(state->ExternalTemperature);
            message.data.push_back(0x50);
*/
///*
            message.data[0] = field1.asByte;
            message.data[1] = carState->CoolantTemperature;
            message.data[2] = carState->Odometer.data.leftByte;
            message.data[3] = carState->Odometer.data.middleByte;
            message.data[4] = carState->Odometer.data.rightByte;
            message.data[5] = carState->ExternalTemperature;
            message.data[6] = carState->ExternalTemperature;
            message.data[7] = field8.asByte;

//*/
            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {
            const auto* tmp = reinterpret_cast<const Can0F6Dash1Struct*>(message.data);

            carState->IsReverseEngaged    = tmp->LightsStatus.data.reverse_gear_light;
            carState->ExternalTemperature = tmp->ExternalTemperature;
            carState->CoolantTemperature  = tmp->CoolantTemperature;

            carState->EngineStatus       = tmp->IgnitionField.data.engine_status;
            carState->EngineRunning      = tmp->IgnitionField.data.engine_status == 2;
            //carState->TrailerPresent     = tmp->IgnitionField.data.config_mode == 0 ? 1 : 0;
            carState->Ignition           = tmp->IgnitionField.data.key_position > 0;
            carState->KeyPosition        = tmp->IgnitionField.data.key_position;
            carState->FactoryMode        = tmp->IgnitionField.data.factory_mode;

            carState->WiperStatus                               = tmp->LightsStatus.data.wiper_status;
            carState->CarSignalLights.data.left_turn_indicator  = tmp->LightsStatus.data.turn_left_light;
            carState->CarSignalLights.data.right_turn_indicator = tmp->LightsStatus.data.turn_right_light;

            carState->Odometer.data.leftByte   = tmp->MileageByte1;
            carState->Odometer.data.middleByte = tmp->MileageByte2;
            carState->Odometer.data.rightByte  = tmp->MileageByte3;
        }
};
#endif
