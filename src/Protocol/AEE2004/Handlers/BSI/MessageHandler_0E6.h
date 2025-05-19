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

        BusMessage Generate(CarState* state)
        {
            message.data[1] = state->RearLeftWheelCounter.data.leftByte;
            message.data[2] = state->RearLeftWheelCounter.data.rightByte;
            message.data[3] = state->RearRightWheelCounter.data.leftByte;
            message.data[4] = state->RearRightWheelCounter.data.rightByte;

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {
            //Can0E6Struct tmp;
            //std::memcpy(&tmp, message.data, static_cast<std::size_t>(sizeof(tmp)));
            const auto* tmp = reinterpret_cast<const Can0E6Struct*>(message.data);

            //TODO
            carState->ABRFault             = tmp->Byte1.data.abr_fault;
            carState->ABSInProgess         = tmp->Byte1.data.abs_in_progress;
            carState->BrakeFluidLevelAlert = tmp->Byte1.data.brake_fluid_level_alert;
            carState->BrakePadsWorn        = tmp->Byte1.data.brake_pads_worn;
            //_carState->BrakePadsWorn = tmp.Byte1.data.ree_fault;
            //_carState->BrakePadsWorn = tmp.Byte1.data.slip_alert;

            carState->RearLeftWheelCounter.asUint16  = tmp->RearLeftWheel.as2Byte;
            carState->RearRightWheelCounter.asUint16 = tmp->RearRightWheel.as2Byte;

            //_canMessageHandlerContainer2010->SetData(0x0E6);
        }
};
#endif
