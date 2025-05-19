#pragma once

#ifndef _MessageHandler_120_2010_h
    #define _MessageHandler_120_2010_h

#include <inttypes.h>
#include <memory>
#include "../../../IMessageHandler.hpp"
#include "../../Structs/CAN_120_2010.h"

class MessageHandler_120_2010 : public IMessageHandler<MessageHandler_120_2010>
{
    private:
        BusMessage message
        {
            .id = 0x120,
            .data = {0},
            .dataLength = 8,
            .protocol = ProtocolType::AEE2010,
            .periodicityMs = 1000,
            .offsetMs = 390,
            .isActive = true
        };
    public:
        static constexpr uint32_t MessageId = 0x120;

        BusMessage Generate(CarState* state)
        {
            CAN_120_2010_Byte4Struct field4{};
            field4.data.GearboxFaultRepairNeeded = state->CarIndicatorLights.data.gearbox_fault;

            CAN_120_2010_Byte5Struct field5{};
            field5.data.EngineFaultRepairNeeded = state->CarIndicatorLights.data.mil || state->CarIndicatorLights.data.antipollution_fault;

            CAN_120_2010_Byte7Struct field7{};
            field7.data.SuspensionFaulty = state->CarIndicatorLights.data.serious_suspension_fault;

            message.data[0] = 0xFF;
            message.data[1] = 0x00;
            message.data[2] = 0x00;
            message.data[3] = field4.asByte;
            message.data[4] = field5.asByte;
            message.data[5] = 0x00;
            message.data[6] = field7.asByte;
            message.data[7] = 0x00;

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {

        }
};
#endif
