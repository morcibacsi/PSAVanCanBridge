#pragma once

#ifndef _MessageHandler_0F6_2010_h
    #define _MessageHandler_0F6_2010_h

#include <cstdint>

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

        BusMessage Generate(CarState* carState)
        {
            CanDash1_2010_Byte1Struct field1{};

            field1.data.key_position = KEY_POSITION_2010_CONTACT;
            if (carState->USE_IGNITION_SIGNAL_FROM_SOURCE_BUS)
            {
                field1.data.key_position = carState->KeyPosition;
            }

            field1.data.engine_status    = carState->EngineStatus;
            field1.data.generator_status = carState->EngineRunning;
            field1.data.factory_mode     = carState->FactoryMode;
            field1.data.config_mode      = carState->FactoryMode == 1 ? CONFIG_MODE_2010_FACTORY : CONFIG_MODE_2010_CLIENT;

            CanDash1_2010_Byte8Struct field8{};
            field8.data.reverse_gear_light = carState->IsReverseEngaged;
            field8.data.wiper_status       = carState->WiperStatus;
            field8.data.turn_left_light    = carState->CarSignalLights.data.left_turn_indicator;
            field8.data.turn_right_light   = carState->CarSignalLights.data.right_turn_indicator;

            message.data[0] = field1.asByte;
            message.data[1] = carState->CoolantTemperature;
            message.data[2] = carState->Odometer.data.leftByte;
            message.data[3] = carState->Odometer.data.middleByte;
            message.data[4] = carState->Odometer.data.rightByte;
            message.data[5] = carState->ExternalTemperature;
            message.data[6] = carState->ExternalTemperature;
            message.data[7] = field8.asByte;

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {

        }
};
#endif
