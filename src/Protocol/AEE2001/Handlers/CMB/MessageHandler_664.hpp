#pragma once

#ifndef _MessageHandler_664_h
    #define _MessageHandler_664_h

#include <cstdint>
#include <cstring>

#include "../../Structs/VAN_664.h"
#include "../../../AEE2004/Structs/CAN_10B_2004.h"
#include "../../../IMessageHandler.hpp"

class MessageHandler_664 : public IMessageHandler<MessageHandler_664>
{
    public:
        static constexpr uint32_t MessageId = 0x664;

        BusMessage Generate(CarState* carState)
        {
            BusMessage message{};
            message.id = MessageId;
            message.periodicityMs = 50;
            message.offsetMs = 20;
            message.protocol = ProtocolType::AEE2001;
            message.type = MessageType::Normal;
            message.isActive = false;
            message.ack = true;

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {
            constexpr std::size_t ExpectedPacketSize = sizeof(VAN_664_2001Struct);

            if (message.dataLength != ExpectedPacketSize)
            {
                return;
            }

            VAN_664_2001Struct packet;
            std::memcpy(&packet, message.data, ExpectedPacketSize);

            if (carState->FuelLevel == 0xFF && (packet.FuelLevelFiltered != 0xFF || packet.FuelLevelUnfiltered != 0xFF))
            {
                uint8_t fuelLevel = packet.FuelLevelFiltered;

                if (fuelLevel == 0xFF)
                {
                    fuelLevel = packet.FuelLevelUnfiltered;
                }

                if (fuelLevel == 0xFF)
                {
                    return;
                }

                carState->FuelLevel = fuelLevel > 100 ? 100 : fuelLevel;
            }
        }
};
#endif
