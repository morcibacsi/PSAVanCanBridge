#pragma once

#ifndef _MessageHandler_2B6_2010_h
    #define _MessageHandler_2B6_2010_h

#include <inttypes.h>
#include <memory>
#include "../../../IMessageHandler.hpp"

/*
    VIN number
*/
class MessageHandler_2B6_2010 : public IMessageHandler
{
    private:
        BusMessage message
        {
            .id = 0x2B6,
            .data = {0},
            .dataLength = 8,
            .protocol = ProtocolType::AEE2010,
            .periodicityMs = 1000-150,
            .offsetMs = 330,
            .isActive = true
        };
    public:
        BusMessage Generate(std::shared_ptr<CarState> state) override
        {
            message.data[0] = state->VIN_FOR_HEADUNIT[9];
            message.data[1] = state->VIN_FOR_HEADUNIT[10];
            message.data[2] = state->VIN_FOR_HEADUNIT[11];
            message.data[3] = state->VIN_FOR_HEADUNIT[12];
            message.data[4] = state->VIN_FOR_HEADUNIT[13];
            message.data[5] = state->VIN_FOR_HEADUNIT[14];
            message.data[6] = state->VIN_FOR_HEADUNIT[15];
            message.data[7] = state->VIN_FOR_HEADUNIT[16];

            return message;
        }
};
#endif
