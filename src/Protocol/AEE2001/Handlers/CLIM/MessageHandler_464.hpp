#pragma once

#ifndef _MessageHandler_464_h
    #define _MessageHandler_464_h

#include <cstdint>
#include <cstring>

#include "VanCanAirConditionerSpeedMap.hpp"
#include "../../Structs/VAN_464.h"
#include "../../../IMessageHandler.hpp"

class MessageHandler_464 : public IMessageHandler<MessageHandler_464>
{
    VanCanAirConditionerSpeedMap* _vanCanAirConditionerSpeedMap;
    public:
        static constexpr uint32_t MessageId = 0x464;

        MessageHandler_464()
        {
            _vanCanAirConditionerSpeedMap = new VanCanAirConditionerSpeedMap();
        }

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
            constexpr std::size_t ExpectedPacketSize = sizeof(VanAirConditioner1Struct);

            if (message.dataLength != ExpectedPacketSize)
            {
                return;
            }

            VanAirConditioner1Struct packet{};
            std::memcpy(&packet, message.data, ExpectedPacketSize);

            uint8_t isRecyclingOn = packet.Status.data.recycling_on;
            uint8_t isAirConEnabled = 0;
            uint8_t airConFanSpeed = 0;
            uint8_t isHeatingPanelOn = 1;

            if (message.data[0] == 0x00 && (packet.PowerConsumption == 0x00))
            {
                isHeatingPanelOn = 0;
            }
            if (
                   (message.data[0] == 0x00 && (packet.PowerConsumption == 0x00))  // off
                || (message.data[0] == 0x00 && (packet.PowerConsumption == 0x0E))  // off + rear window heating
                || (message.data[0] == 0x01 && (packet.PowerConsumption == 0x0E))  // off + rear window heating toggle
                || (message.data[0] == 0x04 && (packet.PowerConsumption == 0x00))  // off + recycle
                || (message.data[0] == 0x04 && (packet.PowerConsumption == 0x0E))  // off + rear window heating + recycle
                || (message.data[0] == 0x05 && (packet.PowerConsumption == 0x00))  // off + rear window heating + recycle toggle
                || (message.data[0] == 0x05 && (packet.PowerConsumption == 0x0E))  // off + rear window heating + recycle toggle
                )
            {
                //nothing to do
            }
            else
            {
                isAirConEnabled = packet.Status.data.aircon_requested;
                airConFanSpeed = _vanCanAirConditionerSpeedMap->GetFanSpeedFromVANByte(
                    packet.PowerConsumption,
                    carState->AirConditionerStatus.data.IsACCompressorOn,
                    carState->AirConditionerStatus.data.IsWindowHeatingOn,
                    isRecyclingOn
                );
            }

            carState->AirConditionerStatus.data.IsHeatingPanelOn = isHeatingPanelOn;
            carState->AirConditionerStatus.data.IsRecyclingOn = isRecyclingOn;
            carState->AirConditionerStatus.data.IsAirConEnabled = isAirConEnabled;
            carState->AirConditionerStatus.data.FanSpeed = airConFanSpeed;
        }
};
#endif
