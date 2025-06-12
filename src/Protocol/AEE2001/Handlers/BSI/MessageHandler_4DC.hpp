#pragma once

#ifndef _MessageHandler_4DC_h
    #define _MessageHandler_4DC_h

#include <cstdint>
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
            message.id = MessageId;
            message.periodicityMs = 500;
            message.offsetMs = 20;
            message.protocol = ProtocolType::AEE2001;
            message.type = MessageType::Normal;
            message.isActive = false;
            message.ack = true;

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {
            constexpr std::size_t ExpectedPacketSize = sizeof(VanAirConditioner2Struct);

            if (message.dataLength != ExpectedPacketSize)
            {
                return;
            }

            VanAirConditioner2Struct packet{};
            std::memcpy(&packet, message.data, ExpectedPacketSize);

            carState->AirConditionerStatus.data.IsWindowHeatingOn = packet.Status1.rear_window_heating_on;
            carState->AirConditionerStatus.data.IsACCompressorOn = packet.Status1.ac_compressor_auth_on;
        }
};
#endif
