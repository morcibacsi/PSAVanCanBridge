#pragma once

#ifndef _MessageHandler_0F6_2010_h
    #define _MessageHandler_0F6_2010_h

#include <inttypes.h>
#include <memory>
#include "../../Structs/CAN_0F6_2010.h"
#include "../../../IMessageHandler.hpp"

class MessageHandler_0F6_2010 : public IMessageHandler<MessageHandler_0F6_2010>
{
    private:
        BusMessage message
        {
            .id = 0x0F6,
            .data = {0},
            .dataLength = 8,
            .protocol = ProtocolType::AEE2010,
            .periodicityMs = 500-100,
            .offsetMs = 240,
            .isActive = true
        };
    public:
        static constexpr uint32_t MessageId = 0x0F6;

        BusMessage Generate(CarState* state)
        {
            CanDash1_2010_Byte1Struct field1{};

            field1.data.key_position = KEY_POSITION_2010_CONTACT;
            if (state->USE_IGNITION_SIGNAL_FROM_SOURCE_BUS)
            {
                field1.data.key_position = state->KeyPosition;
            }

            field1.data.engine_status    = state->EngineStatus;
            field1.data.generator_status = state->EngineRunning;
            field1.data.factory_mode     = state->FactoryMode;
            field1.data.config_mode      = state->FactoryMode == 1 ? CONFIG_MODE_2010_FACTORY : CONFIG_MODE_2010_CLIENT;

            CanDash1_2010_Byte8Struct field8{};
            field8.data.reverse_gear_light = state->IsReverseEngaged;
            field8.data.wiper_status       = state->WiperStatus;
            field8.data.turn_left_light    = state->CarSignalLights.data.left_turn_indicator;
            field8.data.turn_right_light   = state->CarSignalLights.data.right_turn_indicator;

            message.data[0] = field1.asByte;
            message.data[1] = state->CoolantTemperature;
            message.data[2] = state->Odometer.data.leftByte;
            message.data[3] = state->Odometer.data.middleByte;
            message.data[4] = state->Odometer.data.rightByte;
            message.data[5] = state->ExternalTemperature;
            message.data[6] = state->ExternalTemperature;
            message.data[7] = field8.asByte;

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {

        }
};
#endif
