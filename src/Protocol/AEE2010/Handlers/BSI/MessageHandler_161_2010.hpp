#pragma once

#ifndef _MessageHandler_161_2010_h
    #define _MessageHandler_16_20101_h

#include <cstdint>

#include "../../../IMessageHandler.hpp"
#include "../../Structs/CAN_161_2010.h"

class MessageHandler_161_2010 : public IMessageHandler<MessageHandler_161_2010>
{
    private:
        BusMessage message
        {
            .id = 0x161,
            .data = {0},
            .dataLength = 7,
            .protocol = ProtocolType::AEE2010,
            .periodicityMs = 500,
            .offsetMs = 40,
            .isActive = true
        };
    public:
        static constexpr uint32_t MessageId = 0x161;

        BusMessage Generate(CarState* carState)
        {
            CAN_161_2010_Byte5Struct byte5{};
            byte5.data.fuel_tank_capacity_in_liters = carState->FUEL_TANK_CAPACITY_IN_LITERS;

            message.data[0] = 0x00;
            message.data[1] = 0x00;
            message.data[2] = carState->EngineOilTemperature;
            message.data[3] = carState->FuelLevel;
            message.data[4] = byte5.asByte;
            message.data[5] = 0x00;
            message.data[6] = carState->EngineOilLevel;

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {

        }
};
#endif
