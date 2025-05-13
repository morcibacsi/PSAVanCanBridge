#pragma once

#ifndef _MessageHandler_161_2010_h
    #define _MessageHandler_16_20101_h

#include <inttypes.h>
#include <memory>
#include "../../../IMessageHandler.hpp"
#include "../../Structs/CAN_161_2010.h"

class MessageHandler_161_2010 : public IMessageHandler
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
        BusMessage Generate(std::shared_ptr<CarState> state) override
        {
            CAN_161_2010_Byte5Struct byte5{};
            byte5.data.fuel_tank_capacity_in_liters = state->FUEL_TANK_CAPACITY_IN_LITERS;

            message.data[0] = 0x00;
            message.data[1] = 0x00;
            message.data[2] = state->EngineOilTemperature;
            message.data[3] = state->FuelLevel;
            message.data[4] = byte5.asByte;
            message.data[5] = 0x00;
            message.data[6] = state->EngineOilLevel;

            return message;
        }
};
#endif
