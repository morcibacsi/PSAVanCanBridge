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

        BusMessage Generate(CarState* state)
        {
            //TODO copied from AEE2010
            CanDash1Byte1Struct field1{};
            field1.data.config_mode = state->TrailerPresent == 1 ? 0 : 2;

            field1.data.ignition = 1;
            if (state->USE_IGNITION_SIGNAL_FROM_SOURCE_BUS)
            {
                field1.data.ignition = state->Ignition;
            }

            field1.data.engine_status = state->EngineRunning == 1 ? 2 : 0;
            field1.data.generator_status = state->EngineRunning;

            CanDash1Byte8Struct field8{};
            field8.data.reverse_gear_light = state->IsReverseEngaged;
            field8.data.wiper_status = state->WiperStatus;
            field8.data.turn_left_light = state->CarSignalLights.data.left_turn_indicator;
            field8.data.turn_right_light = state->CarSignalLights.data.right_turn_indicator;

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
            message.data[1] = state->CoolantTemperature;
            message.data[2] = state->Odometer.data.leftByte;
            message.data[3] = state->Odometer.data.middleByte;
            message.data[4] = state->Odometer.data.rightByte;
            message.data[5] = state->ExternalTemperature;
            message.data[6] = state->ExternalTemperature;
            message.data[7] = field8.asByte;

//*/
            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {
            //Can0F6Dash1Struct tmp;
            //std::memcpy(&tmp, message.data, static_cast<std::size_t>(sizeof(tmp)));
            const auto* tmp = reinterpret_cast<const Can0F6Dash1Struct*>(message.data);

            carState->IsReverseEngaged    = tmp->LightsStatus.data.reverse_gear_light;
            carState->ExternalTemperature = tmp->ExternalTemperature;
            carState->CoolantTemperature  = tmp->CoolantTemperature;
            //TODO check if this is correct
            carState->EngineStatus       = tmp->IgnitionField.data.engine_status;
            carState->EngineRunning      = tmp->IgnitionField.data.engine_status == 2;
            carState->TrailerPresent     = tmp->IgnitionField.data.config_mode == 0 ? 1 : 0;
            carState->Ignition           = tmp->IgnitionField.data.ignition;

            carState->IsReverseEngaged                          = tmp->LightsStatus.data.reverse_gear_light;
            carState->WiperStatus                               = tmp->LightsStatus.data.wiper_status;
            carState->CarSignalLights.data.left_turn_indicator  = tmp->LightsStatus.data.turn_left_light;
            carState->CarSignalLights.data.right_turn_indicator = tmp->LightsStatus.data.turn_right_light;

            carState->Odometer.data.leftByte   = tmp->MileageByte1;
            carState->Odometer.data.middleByte = tmp->MileageByte2;
            carState->Odometer.data.rightByte  = tmp->MileageByte3;
        }
};
#endif
