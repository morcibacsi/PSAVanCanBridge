#pragma once

#ifndef _MessageHandler_1E3_h
    #define _MessageHandler_1E3_h

#include <inttypes.h>
#include <memory>
#include <cstring>
#include "../../../IMessageHandler.hpp"
#include "../../Structs/CAN_1E3.h"

class MessageHandler_1E3 : public IMessageHandler<MessageHandler_1E3>
{
    private:
        BusMessage message
        {
            .id = 0x1E3,
            .data = {0},
            .dataLength = 7,
            .protocol = ProtocolType::AEE2004,
            .periodicityMs = 200,
            .offsetMs = 25,
            .isActive = true
        };

    public:
        static constexpr uint32_t MessageId = 0x1E3;

        BusMessage Generate(CarState* carState)
        {
            uint8_t fanSpeed = 0x0F;
            CanAirConOnDisplayByte1Struct byte1{};
            CanAirConOnDisplayByte2Struct byte2{};

            if (carState->SEND_AC_CHANGES_TO_DISPLAY)
            {
                //byte1.data.ac_compressor_off = carState->IsACCompressorOn == 1 ? 0 : 1;
                byte1.data.ac_compressor_off = (carState->AirConditionerStatus.data.IsAirConEnabled == 1) ? 0 : 1;
                //byte1.data.off = (carState->Ignition == 1 && carState->IsHeatingPanelOn == 1) ? 0 : 1;
                byte1.data.off = (carState->AirConditionerStatus.data.IsHeatingPanelOn == 1) ? 0 : 1;// (carState->Ignition == 1 && carState->IsHeatingPanelOn == 1) ? 0 : 1;
                byte1.data.recycling_on = carState->AirConditionerStatus.data.IsRecyclingOn;
                byte1.data.recycling_on_pushed = carState->AirConditionerStatus.data.IsRecyclingOn;

                byte2.data.rear_window_heating_on = carState->AirConditionerStatus.data.IsWindowHeatingOn;
                byte2.data.windshield = carState->AirConditionerStatus.data.IsWindowHeatingOn;
            }

            if (carState->SEND_AC_FAN_CHANGES_TO_DISPLAY)
            {
                fanSpeed = carState->AirConditionerStatus.data.FanSpeed;
            }

            message.data[0] = byte1.asByte;
            message.data[1] = byte2.asByte;
            message.data[2] = 0x00;
            message.data[3] = 0x00;
            message.data[4] = 0x00;
            message.data[5] = 0x00;
            message.data[6] = fanSpeed;

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {

        }
};
#endif
