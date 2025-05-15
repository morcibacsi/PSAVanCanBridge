#pragma once

#ifndef _MessageHandler_464_h
    #define _MessageHandler_464_h

#include <inttypes.h>
#include <memory>
#include <cstring>

#include "VanCanAirConditionerSpeedMap.h"
#include "../../Structs/VAN_464.h"
#include "../../../IMessageHandler.hpp"

class MessageHandler_464 : public IMessageHandler
{
    VanCanAirConditionerSpeedMap* _vanCanAirConditionerSpeedMap;
    public:
        MessageHandler_464()
        {
            _vanCanAirConditionerSpeedMap = new VanCanAirConditionerSpeedMap();
        }

        BusMessage Generate(CarState* state) override
        {
            BusMessage message;
            message.id = 0x464;
            message.periodicityMs = 500;
            message.offsetMs = 20;
            message.protocol = ProtocolType::AEE2001;

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message) override
        {
            constexpr std::size_t ExpectedPacketSize = sizeof(VanAirConditioner1Struct);

            if (message.dataLength != ExpectedPacketSize)
            {
                return;
            }

            VanAirConditioner1Struct vanPacket{};
            std::memcpy(&vanPacket, message.data, ExpectedPacketSize);

            uint8_t isRecyclingOn = vanPacket.Status.data.recycling_on;
            uint8_t isAirConEnabled = 0;
            uint8_t airConFanSpeed = 0;
            uint8_t isHeatingPanelOn = 1;

            if (message.data[0] == 0x00 && (vanPacket.PowerConsumption == 0x00))
            {
                isHeatingPanelOn = 0;
            }
            if (
                   (message.data[0] == 0x00 && (vanPacket.PowerConsumption == 0x00))  // off
                || (message.data[0] == 0x00 && (vanPacket.PowerConsumption == 0x0E))  // off + rear window heating
                || (message.data[0] == 0x01 && (vanPacket.PowerConsumption == 0x0E))  // off + rear window heating toggle
                || (message.data[0] == 0x04 && (vanPacket.PowerConsumption == 0x00))  // off + recycle
                || (message.data[0] == 0x04 && (vanPacket.PowerConsumption == 0x0E))  // off + rear window heating + recycle
                || (message.data[0] == 0x05 && (vanPacket.PowerConsumption == 0x00))  // off + rear window heating + recycle toggle
                || (message.data[0] == 0x05 && (vanPacket.PowerConsumption == 0x0E))  // off + rear window heating + recycle toggle
                )
            {
                //nothing to do
            }
            else
            {
                isAirConEnabled = vanPacket.Status.data.aircon_requested;
                airConFanSpeed = _vanCanAirConditionerSpeedMap->GetFanSpeedFromVANByte(
                    vanPacket.PowerConsumption,
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
