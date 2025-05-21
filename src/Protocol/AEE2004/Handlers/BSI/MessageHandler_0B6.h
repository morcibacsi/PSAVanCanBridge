#pragma once

#ifndef _MessageHandler_0B6_h
    #define _MessageHandler_0B6_h

#include <inttypes.h>
#include <memory>
#include <cstring>

#include "../../Structs/CAN_0B6_2004.h"
#include "../../../IMessageHandler.hpp"

class MessageHandler_0B6 : public IMessageHandler<MessageHandler_0B6>
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
        static constexpr uint32_t MessageId = 0x0B6;

        BusMessage Generate(CarState* carState)
        {
            message.data[0] = carState->Rpm.data.leftByte;
            message.data[1] = carState->Rpm.data.rightByte;
            message.data[2] = carState->Speed.data.leftByte;
            message.data[3] = carState->Speed.data.rightByte;
            message.data[4] = carState->DistanceForCMB.data.leftByte;
            message.data[5] = carState->DistanceForCMB.data.rightByte;
            message.data[6] = carState->ConsumptionForCMB;
            message.data[7] = 0xD0;

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {
            Can0B6Struct packet;
            constexpr std::size_t ExpectedPacketSize = sizeof(Can0B6Struct);
            std::memcpy(&packet, message.data, sizeof(packet));

            carState->Rpm.data.leftByte             = packet.RpmByte1;
            carState->Rpm.data.rightByte            = packet.RpmByte2;
            carState->Speed.data.leftByte           = packet.SpeedByte1;
            carState->Speed.data.rightByte          = packet.SpeedByte2;
            carState->DistanceForCMB.data.leftByte  = packet.DistanceForCMB1;
            carState->DistanceForCMB.data.rightByte = packet.DistanceForCMB2;
            carState->ConsumptionForCMB             = packet.ConsumptionForCMB;
            carState->SpeedValidationInfo           = packet.Field8.asByte;

            UInt16 speed{};
            speed.data.leftByte  = packet.SpeedByte1;
            speed.data.rightByte = packet.SpeedByte2;
            carState->SpeedInKmh = speed.asUint16 * 0.01;;
        }
};
#endif
