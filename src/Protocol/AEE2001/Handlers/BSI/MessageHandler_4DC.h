#pragma once

#ifndef _MessageHandler_4DC_h
    #define _MessageHandler_4DC_h

#include <inttypes.h>
#include <memory>
#include <cstring>

#include "../../Structs/VAN_4DC.h"
#include "../../../IMessageHandler.hpp"

class MessageHandler_4DC : public IMessageHandler<MessageHandler_4DC>
{
    public:
        static constexpr uint32_t MessageId = 0x4DC;

        BusMessage Generate(CarState* carState)
        {
            BusMessage message;
            message.id = 0x4DC;
            message.periodicityMs = 500;
            message.offsetMs = 20;
            message.protocol = ProtocolType::AEE2001;

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {
            constexpr std::size_t ExpectedPacketSize = sizeof(VanAirConditioner2Struct);

            if (message.dataLength != ExpectedPacketSize)
            {
                return;
            }

            VanAirConditioner2Struct vanPacket{};
            std::memcpy(&vanPacket, message.data, ExpectedPacketSize);

            carState->AirConditionerStatus.data.IsWindowHeatingOn = vanPacket.Status1.rear_window_heating_on;
            carState->AirConditionerStatus.data.IsACCompressorOn = vanPacket.Status1.ac_compressor_auth_on;
        }
};
#endif
