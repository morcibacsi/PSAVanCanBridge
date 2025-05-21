#pragma once

#ifndef _MessageHandler_3B6_2010_h
    #define _MessageHandler_3B6_2010_h

#include <inttypes.h>
#include <memory>
#include "../../../IMessageHandler.hpp"

/*
    VIN number
*/
class MessageHandler_3B6_2010 : public IMessageHandler<MessageHandler_3B6_2010>
{
    private:
        BusMessage message
        {
            .id = 0x3B6,
            .data = {0},
            .dataLength = 6,
            .protocol = ProtocolType::AEE2010,
            .periodicityMs = 1000-150,
            .offsetMs = 380,
            .isActive = true
        };
    public:
        static constexpr uint32_t MessageId = 0x3B6;

        BusMessage Generate(CarState* carState)
        {
            message.data[0] = carState->VIN_FOR_HEADUNIT[3];
            message.data[1] = carState->VIN_FOR_HEADUNIT[4];
            message.data[2] = carState->VIN_FOR_HEADUNIT[5];
            message.data[3] = carState->VIN_FOR_HEADUNIT[6];
            message.data[4] = carState->VIN_FOR_HEADUNIT[7];
            message.data[5] = carState->VIN_FOR_HEADUNIT[8];

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {

        }
};
#endif
