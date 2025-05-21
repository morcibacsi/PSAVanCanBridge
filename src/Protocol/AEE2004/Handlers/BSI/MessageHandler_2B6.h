#pragma once

#ifndef _MessageHandler_2B6_h
    #define _MessageHandler_2B6_h

#include <inttypes.h>
#include <memory>
#include "../../../IMessageHandler.hpp"

/*
    VIN number
*/
class MessageHandler_2B6 : public IMessageHandler<MessageHandler_2B6>
{
    private:
        BusMessage message
        {
            .id = 0x2B6,
            .data = { 0 },
            .dataLength = 8,
            .protocol = ProtocolType::AEE2004,
            .periodicityMs = 1000-150,
            .offsetMs = 330,
            .isActive = true
        };
    public:
        static constexpr uint32_t MessageId = 0x2B6;

        BusMessage Generate(CarState* carState)
        {
            message.data[0] = carState->VIN_FOR_HEADUNIT[9];
            message.data[1] = carState->VIN_FOR_HEADUNIT[10];
            message.data[2] = carState->VIN_FOR_HEADUNIT[11];
            message.data[3] = carState->VIN_FOR_HEADUNIT[12];
            message.data[4] = carState->VIN_FOR_HEADUNIT[13];
            message.data[5] = carState->VIN_FOR_HEADUNIT[14];
            message.data[6] = carState->VIN_FOR_HEADUNIT[15];
            message.data[7] = carState->VIN_FOR_HEADUNIT[16];

            return message;
        }

        void Parse(CarState* carState, const BusMessage& msg)
        {
        }
};
#endif
