#pragma once

#ifndef _MessageHandler_127_2004_h
    #define _MessageHandler_127_2004_h

#include <inttypes.h>
#include <memory>
#include <cstring>

#include "../../Structs/CAN_127.h"
#include "../../../IMessageHandler.hpp"

class MessageHandler_127 : public IMessageHandler<MessageHandler_127>
{
    private:
        BusMessage message
        {
            .id = 0x127,
            .data = { 0 },
            .dataLength = 2,
            .protocol = ProtocolType::AEE2004,
            .periodicityMs = 100,
            .offsetMs = 0,
            .isActive = true
        };
    public:
        static constexpr uint32_t MessageId = 0x127;

        BusMessage Generate(CarState* carState)
        {
            Can2004_127Byte1 field1{};
            field1.data.enable_vth = carState->Ignition == 1;

            message.data[0] = field1.asByte;
            message.data[1] = 0x00;

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {
            //Can127_2004_Struct tmp;
            //std::memcpy(&tmp, message.data, static_cast<std::size_t>(sizeof(tmp)));
            const auto* tmp = reinterpret_cast<const Can127_2004_Struct*>(message.data);

            carState->CarIndicatorLights.data.authorize_vth = tmp->Status.data.enable_vth;

            //_canMessageHandlerContainer2010->SetData(0x168);
        }
};
#endif
