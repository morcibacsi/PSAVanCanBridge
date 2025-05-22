#pragma once

#ifndef _MessageHandler_0E6_2010_h
    #define _MessageHandler_0E6_2010_h

#include <cstdint>

#include "../../../IMessageHandler.hpp"
#include "../../Structs/CAN_0E6_2010.h"

class MessageHandler_0E6_2010 : public IMessageHandler<MessageHandler_0E6_2010>
{
    private:
        BusMessage message
        {
            .id = 0x0E6,
            .data = {0},
            .dataLength = 8,
            .protocol = ProtocolType::AEE2010,
            .periodicityMs = 100,
            .offsetMs = 60,
            .isActive = true
        };

        uint8_t cntr = 0;

        uint8_t chk_esp(uint8_t* buf)
        {
            uint8_t sum = cntr;
            for (uint8_t i = 0; i < 7; i++){
                sum += buf[i] >> 4;
                sum += buf[i] & 0x0F;
            }

            sum = (cntr << 4) | ((0x7FFC - sum) & 0x0F);
            cntr = cntr < 0x0F ? cntr + 1 : 0;

            return sum;
        }

    public:
        static constexpr uint32_t MessageId = 0x0E6;

        BusMessage Generate(CarState* carState)
        {
            CAN_0E6_2010_Byte1Struct byte1{};
            byte1.data.abr_fault = carState->ABRFault;
            byte1.data.brake_fluid_level_alert = carState->BrakeFluidLevelAlert;
            byte1.data.brake_pads_worn = carState->BrakePadsWorn;
            byte1.data.ref_in_progress = 0;
            byte1.data.auto_warning_lights_by_brake_cpu = 0;
            byte1.data.abs_in_progress = carState->ABSInProgess;
            byte1.data.ref_fault = 0;

            message.data[0] = byte1.asByte;
            message.data[1] = carState->RearLeftWheelCounter.data.leftByte;
            message.data[2] = carState->RearLeftWheelCounter.data.rightByte;
            message.data[3] = carState->RearRightWheelCounter.data.leftByte;
            message.data[4] = carState->RearRightWheelCounter.data.rightByte;
            message.data[5] = 0x83;
            message.data[6] = 0x8C;
            message.data[7] = chk_esp(message.data);

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {

        }
};
#endif
