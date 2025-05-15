#pragma once

#ifndef _MessageHandler_161_h
    #define _MessageHandler_161_h

#include <inttypes.h>
#include <memory>
#include <cstring>

#include "../../Structs/CAN_161_2004.h"
#include "../../../IMessageHandler.hpp"

class MessageHandler_161 : public IMessageHandler
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
        BusMessage Generate(CarState* state) override
        {
            message.data[0] = 0x00;
            message.data[1] = 0x00;
            message.data[2] = state->EngineOilTemperature;
            message.data[3] = state->FuelLevel;
            message.data[4] = 0x00;
            message.data[5] = 0x00;
            message.data[6] = state->EngineOilLevel;

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message) override
        {
            //CAN_161_2004Struct tmp;
            //std::memcpy(&tmp, message.data, static_cast<std::size_t>(sizeof(tmp)));
            const auto* tmp = reinterpret_cast<const CAN_161_2004Struct*>(message.data);

            carState->OilLevelRestart      = tmp->Field1.data.oil_level_restart;
            carState->EngineOilTemperature = tmp->EngineOilTemperature;
            carState->EngineOilLevel       = tmp->EngineOilLevel;
            carState->FuelLevel            = tmp->FuelLevel;

            //_canMessageHandlerContainer2010->SetData(0x161);
        }
};
#endif
