#pragma once

#ifndef _MessageHandler_260_2010_h
    #define _MessageHandler_260_2010_h

#include <inttypes.h>
#include <memory>
#include "../../../IMessageHandler.hpp"
#include "../../Structs/CAN_260_2010.h"

class MessageHandler_260_2010 : public IMessageHandler
{
    private:
        BusMessage message
        {
            .id = 0x260,
            .data = {0},
            .dataLength = 8,
            .protocol = ProtocolType::AEE2010,
            .periodicityMs = 500-250,
            .offsetMs = 440,
            .isActive = true
        };

    public:
        BusMessage Generate(CarState* state) override
        {
            CAN_260_2010_Byte1Struct byte1{};
            byte1.data.consumption_unit = state->CONSUMPTION_UNIT;
            byte1.data.distance_unit    = state->DISTANCE_UNIT;
            byte1.data.language         = state->LANGUAGE;
            byte1.data.unit_and_language_data_valid = 1;

            CAN_260_2010_Byte2Struct byte2{};
            byte2.data.volume_unit      = state->VOLUME_UNIT;
            byte2.data.temperature_unit = state->TEMPERATURE_UNIT;
            byte2.data.ambience_level   = state->AMBIENCE_LEVEL;
            byte2.data.sound_harmony    = state->SOUND_HARMONY;
            byte2.data.vehicle_function_data = 1;

            CAN_260_2010_Byte3Struct byte3{};
            byte3.data.ambience_lighting        = state->AmbientLighting;
            byte3.data.drl                      = state->DRL;
            byte3.data.automatic_electric_brake = state->AutomaticElectricBrake;

            CAN_260_2010_Byte6Struct byte6{};
            byte6.data.braking_on_alarm_risk = state->BreakingOnAlarmRisk;

            message.data[0] = byte1.asByte;
            message.data[1] = byte2.asByte;
            message.data[2] = byte3.asByte;
            message.data[3] = 0x00;
            message.data[4] = 0x00;
            message.data[5] = byte6.asByte;
            message.data[6] = 0x00;
            message.data[7] = 0x00;

            return message;
        }
};
#endif
