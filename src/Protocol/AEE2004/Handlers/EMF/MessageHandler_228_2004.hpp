#pragma once

#ifndef _MessageHandler_228_2004_h
    #define _MessageHandler_228_2004_h

#include <cstdint>

#include "../../../IMessageHandler.hpp"

class MessageHandler_228_2004 : public IMessageHandler<MessageHandler_228_2004>
{
    private:
        BusMessage message
        {
            .id = MessageId,
            .data = {0},
            .dataLength = 2,
            .protocol = ProtocolType::AEE2004,
            .periodicityMs = 1000,
            .offsetMs = 65,
            //TODO
            .isActive = false
        };
    public:
        static constexpr uint32_t MessageId = 0x228;

        BusMessage Generate(CarState* carState)
        {
            message.isActive = carState->HAS_RTC && carState->SEND_TIME;

            message.data[0] = carState->Hour;
            message.data[1] = carState->Minute;

            return message;
        }

        void Parse(CarState* carState, const BusMessage& msg)
        {
        }
};
#endif
