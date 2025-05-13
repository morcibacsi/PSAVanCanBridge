#pragma once

#ifndef _MessageHandler_036_2010_h
    #define _MessageHandler_036_2010_h

#include <inttypes.h>
#include <memory>
#include "../../../IMessageHandler.hpp"
#include "../../Structs/CAN_036_2010.h"

//2010 slight difference in almost all bytes, mostly compatible with 2004

/*
    Ignition data for EMF, radio, telematics, etc. Brightness for CMB
*/
class MessageHandler_036_2010 : public IMessageHandler
{
    private:
        BusMessage message
        {
            .id = 0x036,
            .data = {0},
            .dataLength = 8,
            .protocol = ProtocolType::AEE2010,
            .periodicityMs = 100-30,
            .offsetMs = 0,
            .isActive = true
        };
    public:
        BusMessage Generate(std::shared_ptr<CarState> state) override
        {
            CAN_036_2010_Byte1Struct byte1{};
            byte1.data.driving_direction = state->IsReverseEngaged ? DRIVING_DIRECTION_REVERSE : DRIVING_DIRECTION_FORWARD;

            CAN_036_2010_Byte3Struct byte3{};
            byte3.data.economy_mode_active = state->EconomyMode;

            CAN_036_2010_Byte4Struct byte4{};
            byte4.data.night_mode = state->NightMode;
            byte4.data.dashboard_brightness = state->DashboardBrightness;
            byte4.data.black_panel_status = state->BlackPanelStatus;

            CAN_036_2010_Byte5Struct byte5{};
            byte5.data.ignition_mode = 1;

            if (state->USE_IGNITION_SIGNAL_FROM_SOURCE_BUS)
            {
                byte5.data.ignition_mode = state->IgnitionMode;
            }

            message.data[0] = byte1.asByte;
            message.data[1] = 0x00;
            message.data[2] = byte3.asByte;
            message.data[3] = byte4.asByte;
            message.data[4] = byte5.asByte;
            message.data[5] = 0x00;
            message.data[6] = 0x00;
            message.data[7] = 0xA0;

            return message;
        }
};
#endif
