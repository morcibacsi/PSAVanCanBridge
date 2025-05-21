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
class MessageHandler_036_2010 : public IMessageHandler<MessageHandler_036_2010>
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
        static constexpr uint32_t MessageId = 0x036;

        BusMessage Generate(CarState* carState)
        {
            CAN_036_2010_Byte1Struct byte1{};
            byte1.data.driving_direction = carState->IsReverseEngaged ? DRIVING_DIRECTION_REVERSE : DRIVING_DIRECTION_FORWARD;

            CAN_036_2010_Byte3Struct byte3{};
            byte3.data.economy_mode_active = carState->EconomyMode;

            CAN_036_2010_Byte4Struct byte4{};
            byte4.data.night_mode = carState->NightMode;
            byte4.data.dashboard_brightness = carState->DashboardBrightness;
            byte4.data.black_panel_status = carState->BlackPanelStatus;

            CAN_036_2010_Byte5Struct byte5{};
            byte5.data.ignition_mode = 1;

            if (carState->USE_IGNITION_SIGNAL_FROM_SOURCE_BUS)
            {
                byte5.data.ignition_mode = carState->IgnitionMode;
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

        void Parse(CarState* carState, const BusMessage& message)
        {

        }
};
#endif
