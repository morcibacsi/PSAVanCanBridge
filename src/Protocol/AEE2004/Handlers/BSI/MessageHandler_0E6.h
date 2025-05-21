#pragma once

#ifndef _MessageHandler_0E6_h
    #define _MessageHandler_0E6_h

#include <inttypes.h>
#include <memory>
#include <cstring>

#include "../../Structs/CAN_0E6.h"
#include "../../../IMessageHandler.hpp"

class MessageHandler_0E6 : public IMessageHandler<MessageHandler_0E6>
{
    private:
        BusMessage message
        {
            .id = 0x0E6,
            .command = 0,
            .data = { 0 },
            .dataLength = 5,
            .protocol = ProtocolType::AEE2004,
            .periodicityMs = 100,
            .offsetMs = 60,
            .isActive = true
        };
    public:
        static constexpr uint32_t MessageId = 0x0E6;

        BusMessage Generate(CarState* carState)
        {
            message.data[1] = carState->RearLeftWheelCounter.data.leftByte;
            message.data[2] = carState->RearLeftWheelCounter.data.rightByte;
            message.data[3] = carState->RearRightWheelCounter.data.leftByte;
            message.data[4] = carState->RearRightWheelCounter.data.rightByte;

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {
            Can0E6Struct packet;
            std::memcpy(&packet, message.data, sizeof(packet));

            //TODO
            carState->ABRFault             = packet.Byte1.data.abr_fault;
            carState->ABSInProgess         = packet.Byte1.data.abs_in_progress;
            carState->BrakeFluidLevelAlert = packet.Byte1.data.brake_fluid_level_alert;
            carState->BrakePadsWorn        = packet.Byte1.data.brake_pads_worn;
            //_carState->BrakePadsWorn       = packet.Byte1.data.ree_fault;
            //_carState->BrakePadsWorn       = packet.Byte1.data.slip_alert;

            carState->RearLeftWheelCounter.asUint16  = packet.RearLeftWheel.as2Byte;
            carState->RearRightWheelCounter.asUint16 = packet.RearRightWheel.as2Byte;
        }
};
#endif
