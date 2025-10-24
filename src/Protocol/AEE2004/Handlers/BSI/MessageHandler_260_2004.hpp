#pragma once

#ifndef _MessageHandler_260_2004_h
    #define _MessageHandler_260_2004_h

#include <cstdint>
#include <cstring>

#include "../../../IMessageHandler.hpp"

class MessageHandler_260_2004 : public IMessageHandler<MessageHandler_260_2004>
{
    private:
        BusMessage message
        {
            .id = 0x260,
            .data = {0},
            .dataLength = 8,
            .protocol = ProtocolType::AEE2004,
            .periodicityMs = 500-10,
            .offsetMs = 730,
            .isActive = false
        };

    public:
        static constexpr uint32_t MessageId = 0x260;

        BusMessage Generate(CarState* carState)
        {
            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {
            carState->CarSettings.Field1.asByte = message.data[0];
            carState->CarSettings.Field2.asByte = message.data[1];
            carState->CarSettings.Field3.asByte = message.data[2];
            carState->CarSettings.Field4.asByte = message.data[3];
            carState->CarSettings.Field5.asByte = message.data[4];
            carState->CarSettings.Field6.asByte = message.data[5];
            carState->CarSettings.Field7.asByte = message.data[6];
            carState->CarSettings.Field8.asByte = message.data[7];
        }
};
#endif
