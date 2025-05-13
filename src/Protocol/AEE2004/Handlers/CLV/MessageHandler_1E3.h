#pragma once

#ifndef _MessageHandler_1E3_h
    #define _MessageHandler_1E3_h

#include <inttypes.h>
#include <memory>
#include <cstring>
#include "../../../IMessageHandler.hpp"
#include "../../Structs/CAN_1E3.h"

class MessageHandler_1E3 : public IMessageHandler
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
        BusMessage Generate(std::shared_ptr<CarState> state) override
        {
            uint8_t fanSpeed = 0x0F;
            CanAirConOnDisplayByte1Struct byte1{};
            CanAirConOnDisplayByte2Struct byte2{};

            if (state->SEND_AC_CHANGES_TO_DISPLAY)
            {
                //byte1.data.ac_compressor_off = state->IsACCompressorOn == 1 ? 0 : 1;
                byte1.data.ac_compressor_off = (state->AirConditionerStatus.data.IsAirConEnabled == 1) ? 0 : 1;
                //byte1.data.off = (state->Ignition == 1 && state->IsHeatingPanelOn == 1) ? 0 : 1;
                byte1.data.off = (state->AirConditionerStatus.data.IsHeatingPanelOn == 1) ? 0 : 1;// (state->Ignition == 1 && state->IsHeatingPanelOn == 1) ? 0 : 1;
                byte1.data.recycling_on = state->AirConditionerStatus.data.IsRecyclingOn;
                byte1.data.recycling_on_pushed = state->AirConditionerStatus.data.IsRecyclingOn;

                byte2.data.rear_window_heating_on = state->AirConditionerStatus.data.IsWindowHeatingOn;
                byte2.data.windshield = state->AirConditionerStatus.data.IsWindowHeatingOn;
            }

            if (state->SEND_AC_FAN_CHANGES_TO_DISPLAY)
            {
                fanSpeed = state->AirConditionerStatus.data.FanSpeed;
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

        void Parse(std::shared_ptr<CarState> carState, const BusMessage& message) override
        {

        }
};
#endif
