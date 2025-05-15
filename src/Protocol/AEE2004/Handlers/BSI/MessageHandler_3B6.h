#pragma once

#ifndef _MessageHandler_3B6_h
    #define _MessageHandler_3B6_h

#include <inttypes.h>
#include <memory>
#include "../../../IMessageHandler.hpp"

/*
    VIN number
*/
class MessageHandler_3B6 : public IMessageHandler
{
    private:
        BusMessage message
        {
            .id = 0x3B6,
            .data = { 0 },
            .dataLength = 6,
            .protocol = ProtocolType::AEE2004,
            .periodicityMs = 1000-150,
            .offsetMs = 380,
            .isActive = true
        };

    public:
        BusMessage Generate(CarState* state) override
        {
            message.data[0] = state->VIN_FOR_HEADUNIT[3];
            message.data[1] = state->VIN_FOR_HEADUNIT[4];
            message.data[2] = state->VIN_FOR_HEADUNIT[5];
            message.data[3] = state->VIN_FOR_HEADUNIT[6];
            message.data[4] = state->VIN_FOR_HEADUNIT[7];
            message.data[5] = state->VIN_FOR_HEADUNIT[8];

            return message;
        }
};
#endif
