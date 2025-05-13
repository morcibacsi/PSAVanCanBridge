#pragma once

#ifndef _MessageHandler_336_h
    #define _MessageHandler_336_h

#include <inttypes.h>
#include <memory>
#include "../../../IMessageHandler.hpp"

/*
    VIN number
*/
class MessageHandler_336 : public IMessageHandler
{
    private:
        BusMessage message
        {
            .id = 0x336,
            .data = { 0 },
            .dataLength = 3,
            .protocol = ProtocolType::AEE2004,
            .periodicityMs = 1000-150,
            .offsetMs = 780,
            .isActive = true
        };

    public:
        BusMessage Generate(std::shared_ptr<CarState> state) override
        {
            message.data[0] = state->VIN_FOR_HEADUNIT[0];
            message.data[1] = state->VIN_FOR_HEADUNIT[1];
            message.data[2] = state->VIN_FOR_HEADUNIT[2];

            return message;
        }
};
#endif
