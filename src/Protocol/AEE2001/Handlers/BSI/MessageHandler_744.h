#pragma once

#ifndef _MessageHandler_744_h
    #define _MessageHandler_744_h

#include <inttypes.h>
#include <memory>
#include <cstring>

#include "../../Structs/VAN_744.h"
#include "../../../AEE2004/Structs/CAN_10B.h"
#include "../../../IMessageHandler.hpp"

class MessageHandler_744 : public IMessageHandler<MessageHandler_744>
{
    public:
        static constexpr uint32_t MessageId = 0x744;

        BusMessage Generate(CarState* state)
        {
            BusMessage message;
            message.id = 0x744;
            message.periodicityMs = 50;
            message.offsetMs = 20;
            message.protocol = ProtocolType::AEE2001;

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {
            constexpr std::size_t ExpectedPacketSize = sizeof(VanRearWheelDataStructs);

            if (message.dataLength != ExpectedPacketSize)
            {
                return;
            }

            VanRearWheelDataStructs vanPacket;
            std::memcpy(&vanPacket, message.data, ExpectedPacketSize);

            CAN_10B_Byte4Struct canStatus;
            canStatus.asByte = vanPacket.SteeringStatus.asByte;
            canStatus.data.rotation_direction = vanPacket.SteeringStatus.data.rotation_direction == 0 ? 1 : 0;

            carState->RearLeftWheelCounter.data.leftByte = vanPacket.RearLeftWheelCounter1;
            carState->RearLeftWheelCounter.data.rightByte = vanPacket.RearLeftWheelCounter2;
            carState->RearRightWheelCounter.data.leftByte = vanPacket.RearRightWheelCounter1;
            carState->RearRightWheelCounter.data.rightByte = vanPacket.RearRightWheelCounter2;

            carState->SteeringAngle.data.leftByte = vanPacket.SteeringAngle1;
            carState->SteeringAngle.data.rightByte = vanPacket.SteeringAngle2;
            carState->SteeringSpeed = vanPacket.SteeringSpeed;
            carState->SteeringStatus = canStatus.asByte;
        }
};
#endif
