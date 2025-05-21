#pragma once

#ifndef _MessageHandler_122_2010_h
    #define _MessageHandler_122_2010_h

#include <inttypes.h>
#include <memory>
#include "../../../IMessageHandler.hpp"

/*
    Cruise control related data
*/
class MessageHandler_122_2010 : public IMessageHandler<MessageHandler_122_2010>
{
    private:
        BusMessage message
        {
            .id = 0x122,
            .data = { 0 },
            .dataLength = 8,
            .protocol = ProtocolType::AEE2010,
            .periodicityMs = 200,
            .offsetMs = 0,
            .isActive = true
        };
    public:
        static constexpr uint32_t MessageId = 0x122;

        BusMessage Generate(CarState* carState)
        {
            message.data[0] = 0x00;
            message.data[1] = 0x00;
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
