#pragma once

#ifndef _MessageHandler_336_2010_h
    #define _MessageHandler_336_2010_h

#include <cstdint>

#include "../../../IMessageHandler.hpp"

/*
    VIN number
*/
class MessageHandler_336_2010 : public IMessageHandler<MessageHandler_336_2010>
{
    private:
        BusMessage message
        {
            .id = 0x336,
            .data = {0},
            .dataLength = 3,
            .protocol = ProtocolType::AEE2010,
            .periodicityMs = 1000-150,
            .offsetMs = 780,
            .isActive = true
        };

    public:
        static constexpr uint32_t MessageId = 0x336;

        BusMessage Generate(CarState* carState)
        {
            message.data[0] = carState->VIN_FOR_HEADUNIT[0];
            message.data[1] = carState->VIN_FOR_HEADUNIT[1];
            message.data[2] = carState->VIN_FOR_HEADUNIT[2];

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {

        }
};
#endif
