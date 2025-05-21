#pragma once

#ifndef _MessageHandler_161_h
    #define _MessageHandler_161_h

#include <inttypes.h>
#include <memory>
#include <cstring>

#include "../../Structs/CAN_161_2004.h"
#include "../../../IMessageHandler.hpp"

class MessageHandler_161 : public IMessageHandler<MessageHandler_161>
{
    private:
        BusMessage message
        {
            .id = 0x161,
            .data = {0},
            .dataLength = 7,
            .protocol = ProtocolType::AEE2004,
            .periodicityMs = 500,
            .offsetMs = 40,
            .isActive = true
        };
    public:
        static constexpr uint32_t MessageId = 0x161;

        BusMessage Generate(CarState* carState)
        {
            message.data[0] = 0x00;
            message.data[1] = 0x00;
            message.data[2] = carState->EngineOilTemperature;
            message.data[3] = carState->FuelLevel;
            message.data[4] = 0x00;
            message.data[5] = 0x00;
            message.data[6] = carState->EngineOilLevel;

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {
            CAN_161_2004Struct packet;
            std::memcpy(&packet, message.data, sizeof(packet));

            carState->OilLevelRestart      = packet.Field1.data.oil_level_restart;
            carState->EngineOilTemperature = packet.EngineOilTemperature;
            carState->EngineOilLevel       = packet.EngineOilLevel;
            carState->FuelLevel            = packet.FuelLevel;
        }
};
#endif
