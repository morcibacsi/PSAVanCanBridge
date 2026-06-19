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
        uint8_t activeChecksumCounter = 0;
        uint64_t lastChecksumCounterUpdateTime = 0;
        bool checksumCounterInitialized = false;

        uint8_t chk_esp(uint8_t* buf, uint8_t counter)
        {
            uint8_t sum = counter;
            for (uint8_t i = 0; i < 7; i++){
                sum += buf[i] >> 4;
                sum += buf[i] & 0x0F;
            }

            sum = (counter << 4) | ((0x7FFC - sum) & 0x0F);

            return sum;
        }

        void updateChecksumCounter(CarState* carState)
        {
            if (!checksumCounterInitialized || carState->CurrenTime - lastChecksumCounterUpdateTime >= message.periodicityMs)
            {
                activeChecksumCounter = cntr;
                cntr = cntr < 0x0F ? cntr + 1 : 0;
                lastChecksumCounterUpdateTime = carState->CurrenTime;
                checksumCounterInitialized = true;
            }
        }

    public:
        static constexpr uint32_t MessageId = 0x0E6;

        BusMessage Generate(CarState* carState)
        {
            updateChecksumCounter(carState);

            CAN_0E6_2010_Byte1Struct byte1{};
            byte1.data.abr_fault                        = carState->ABRFault;
            byte1.data.brake_fluid_level_alert          = carState->BrakeFluidLevelAlert;
            byte1.data.brake_pads_worn                  = carState->BrakePadsWorn;
            byte1.data.ref_in_progress                  = 0;
            byte1.data.auto_warning_lights_by_brake_cpu = 0;
            byte1.data.abs_in_progress                  = carState->ABSInProgess;
            byte1.data.ref_fault                        = 0;

            uint8_t leftWheelLeftByte   = carState->RearLeftWheelCounter.data.leftByte;
            uint8_t leftWheelRightByte  = carState->RearLeftWheelCounter.data.rightByte;
            uint8_t rightWheelLeftByte  = carState->RearRightWheelCounter.data.leftByte;
            uint8_t rightWheelRightByte = carState->RearRightWheelCounter.data.rightByte;

            if (leftWheelLeftByte == 0xFF && leftWheelRightByte == 0xFF && rightWheelLeftByte == 0xFF && rightWheelRightByte == 0xFF)
            {
                leftWheelLeftByte   = carState->DistanceForCMB.data.leftByte;
                leftWheelRightByte  = carState->DistanceForCMB.data.rightByte;
                rightWheelLeftByte  = carState->DistanceForCMB.data.leftByte;
                rightWheelRightByte = carState->DistanceForCMB.data.rightByte;
            }

            message.data[0] = byte1.asByte;
            message.data[1] = leftWheelLeftByte;
            message.data[2] = leftWheelRightByte;
            message.data[3] = rightWheelLeftByte;
            message.data[4] = rightWheelRightByte;
            message.data[5] = 0x8E;
            message.data[6] = 0x80;
            message.data[7] = chk_esp(message.data, activeChecksumCounter);

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {

        }
};
#endif
