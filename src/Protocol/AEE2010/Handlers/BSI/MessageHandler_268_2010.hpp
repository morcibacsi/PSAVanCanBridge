#pragma once

#ifndef _MessageHandler_268_2010_h
    #define _MessageHandler_268_2010_h

#include <cstdint>

#include "../../../IMessageHandler.hpp"
#include "../../Structs/CAN_268_2010.h"

class MessageHandler_268_2010 : public IMessageHandler<MessageHandler_268_2010>
{
    private:
        BusMessage message
        {
            .id = 0x268,
            .data = {0},
            .dataLength = 8,
            .protocol = ProtocolType::AEE2010,
            .periodicityMs = 1000-200,
            .offsetMs = 160,
            .isActive = true
        };

    public:
        static constexpr uint32_t MessageId = 0x268;

        BusMessage Generate(CarState* carState)
        {
            float threshold = carState->SpeedLimitFromNacInKmh * (1.0f + carState->SPEED_SIGN_SPEED_TOLERANCE_PERCENT / 100.0f);

            CAN_268_2010_Byte2Struct byte2{};
            byte2.data.roadsign_type              = 0;
            byte2.data.speed_info_high_confidence = 1;
            byte2.data.overspeed                  = carState->OdometerStates.data.SpeedDisplayedOnCmb > threshold ? 1 : 0;

            message.data[0] = carState->SpeedLimitFromNacInKmh;
            message.data[1] = byte2.asByte;
            message.data[2] = 0x00;
            message.data[3] = 0x00;
            message.data[4] = 0x00;
            message.data[5] = 0x00;
            message.data[6] = 0x00;
            message.data[7] = 0x00;

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {

        }
};
#endif
