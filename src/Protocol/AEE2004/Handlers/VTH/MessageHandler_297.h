#pragma once

#ifndef _MessageHandler_297_h
    #define _MessageHandler_297_h

#include <inttypes.h>
#include <memory>
#include "../../../IMessageHandler.hpp"
#include "../../Structs/CAN_297.h"

class MessageHandler_297 : public IMessageHandler<MessageHandler_297>
{
    private:
        BusMessage message
        {
            .id = 0x297,
            .data = { 0 },
            .dataLength = 2,
            .protocol = ProtocolType::AEE2004,
            .periodicityMs = 200,
            .offsetMs = 0,
            //TODO
            .isActive = false
        };

    public:
        static constexpr uint32_t MessageId = 0x297;

        BusMessage Generate(CarState* state)
        {
            Can2004_297Byte1 status{};
            status.data.is_active = state->Ignition;

            message.data[0] = status.asByte;
            message.data[1] = state->Speed.asUint16 * 0.01;

            return message;
        }

        void Parse(CarState* state, const BusMessage& msg)
        {
        }
};
#endif
