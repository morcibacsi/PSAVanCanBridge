#pragma once

#ifndef _MessageHandler_0B6_h
    #define _MessageHandler_0B6_h

#include <inttypes.h>
#include <memory>
#include <cstring>

#include "../../Structs/CAN_0B6_2004.h"
#include "../../../IMessageHandler.hpp"

class MessageHandler_0B6 : public IMessageHandler
{
    private:
        BusMessage message
        {
            .id = 0x0B6,
            .data = {0},
            .dataLength = 8,
            .protocol = ProtocolType::AEE2004,
            .periodicityMs = 100,
            .offsetMs = 20,
            .isActive = true
        };

    public:
        BusMessage Generate(std::shared_ptr<CarState> state) override
        {
            message.data[0] = state->Rpm.data.leftByte;
            message.data[1] = state->Rpm.data.rightByte;
            message.data[2] = state->Speed.data.leftByte;
            message.data[3] = state->Speed.data.rightByte;
            message.data[4] = state->DistanceForCMB.data.leftByte;
            message.data[5] = state->DistanceForCMB.data.rightByte;
            message.data[6] = state->ConsumptionForCMB;
            message.data[7] = 0xD0;

            return message;
        }

        void Parse(std::shared_ptr<CarState> carState, const BusMessage& message) override
        {
            //Can0B6Struct tmp;
            //constexpr std::size_t ExpectedPacketSize = sizeof(Can0B6Struct);
            //std::memcpy(&tmp, message.data, static_cast<std::size_t>(sizeof(tmp)));
            const auto* tmp = reinterpret_cast<const Can0B6Struct*>(message.data);

            carState->Rpm.data.leftByte             = tmp->RpmByte1;
            carState->Rpm.data.rightByte            = tmp->RpmByte2;
            carState->Speed.data.leftByte           = tmp->SpeedByte1;
            carState->Speed.data.rightByte          = tmp->SpeedByte2;
            carState->DistanceForCMB.data.leftByte  = tmp->DistanceForCMB1;
            carState->DistanceForCMB.data.rightByte = tmp->DistanceForCMB2;
            carState->ConsumptionForCMB             = tmp->ConsumptionForCMB;
            carState->SpeedValidationInfo           = tmp->Field8.asByte;

            UInt16 speed{};
            speed.data.leftByte  = tmp->SpeedByte1;
            speed.data.rightByte = tmp->SpeedByte2;
            carState->SpeedInKmh = speed.asUint16 * 0.01;;
        }
};
#endif
