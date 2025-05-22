#pragma once

#ifndef _MessageHandler_744_h
    #define _MessageHandler_744_h

#include <cstdint>
#include <cstring>

#include "../../Structs/VAN_744.h"
#include "../../../AEE2004/Structs/CAN_10B_2004.h"
#include "../../../IMessageHandler.hpp"

class MessageHandler_744 : public IMessageHandler<MessageHandler_744>
{
    public:
        static constexpr uint32_t MessageId = 0x744;

        BusMessage Generate(CarState* carState)
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

            VanRearWheelDataStructs packet;
            std::memcpy(&packet, message.data, ExpectedPacketSize);

            //TODO: remove the dependency on AEE2004
            CAN_10B_Byte4Struct canStatus;
            canStatus.asByte                  = packet.SteeringStatus.asByte;
            canStatus.data.rotation_direction = packet.SteeringStatus.data.rotation_direction == 0 ? 1 : 0;

            carState->RearLeftWheelCounter.data.leftByte   = packet.RearLeftWheelCounter1;
            carState->RearLeftWheelCounter.data.rightByte  = packet.RearLeftWheelCounter2;
            carState->RearRightWheelCounter.data.leftByte  = packet.RearRightWheelCounter1;
            carState->RearRightWheelCounter.data.rightByte = packet.RearRightWheelCounter2;

            carState->SteeringAngle.data.leftByte  = packet.SteeringAngle1;
            carState->SteeringAngle.data.rightByte = packet.SteeringAngle2;

            carState->SteeringSpeed  = packet.SteeringSpeed;
            carState->SteeringStatus = canStatus.asByte;
        }
};
#endif
