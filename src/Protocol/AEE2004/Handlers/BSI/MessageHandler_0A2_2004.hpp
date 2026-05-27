#pragma once

#ifndef _MessageHandler_0A2_2004_h
    #define _MessageHandler_0A2_2004_h

#include <cstdint>
#include <cstring>

#include "../../../IMessageHandler.hpp"
#include "../../Structs/CAN_0A2_2004.h"

class MessageHandler_0A2_2004 : public IMessageHandler<MessageHandler_0A2_2004>
{
    private:
        BusMessage message
        {
            .id = 0x0A2,
            .data = {0},
            .dataLength = 5,
            .protocol = ProtocolType::AEE2004,
            .periodicityMs = 100,
            .offsetMs = 80,
            .isActive = true
        };
    public:
        static constexpr uint32_t MessageId = 0x0A2;

        BusMessage Generate(CarState* carState)
        {
            CAN_0A2_2004_Byte2Struct navCommands{};
            navCommands.data.voice_command     = carState->SteeringWheelRemote.data.voice_command;
            navCommands.data.left_push         = carState->SteeringWheelRemote.data.left_push;
            navCommands.data.mode_button       = carState->SteeringWheelRemote.data.mode_button;
            navCommands.data.menu_button       = carState->SteeringWheelRemote.data.menu_button;
            navCommands.data.escape_button     = carState->SteeringWheelRemote.data.escape_button;
            navCommands.data.validation_button = carState->SteeringWheelRemote.data.validation_button;
            navCommands.data.scroll_underflow  = carState->SteeringWheelRemote.data.scroll_underflow;
            navCommands.data.scroll_overflow   = carState->SteeringWheelRemote.data.scroll_overflow;

            CAN_0A2_2004_Byte3Struct phoneCommands{};
            phoneCommands.data.telephone_command = carState->SteeringWheelRemote.data.telephone_command;

            CAN_0A2_2004_Byte5Struct cmbCommands{};
            cmbCommands.data.volume_knob_underflow = carState->SteeringWheelRemote.data.volume_knob_underflow;
            cmbCommands.data.volume_knob_overflow  = carState->SteeringWheelRemote.data.volume_knob_overflow;
            cmbCommands.data.ac_recycling_command  = carState->SteeringWheelRemote.data.ac_recycling_command;
            cmbCommands.data.cmb_escape_button     = carState->SteeringWheelRemote.data.cmb_escape_button;
            cmbCommands.data.cmb_validation        = carState->SteeringWheelRemote.data.cmb_validation;
            cmbCommands.data.cmb_underflow         = carState->SteeringWheelRemote.data.cmb_underflow;
            cmbCommands.data.cmb_overflow          = carState->SteeringWheelRemote.data.cmb_overflow;

            uint8_t cmbNavMenuCounter = carState->SteeringWheelRemote.data.CmbNavMenuCounter;
            uint8_t navMenuScroll     = carState->SteeringWheelRemote.data.NavMenuScroll;

            if (carState->EMULATE_STEERING_WHEEL_CONTROLS_WITH_STALK)
            {
                navCommands.data.scroll_underflow = carState->RadioRemote.data.owerflow_scan_negative;
                navCommands.data.scroll_overflow  = carState->RadioRemote.data.owerflow_scan_positive;
                cmbCommands.data.cmb_underflow    = carState->RadioRemote.data.owerflow_scan_negative;
                cmbCommands.data.cmb_overflow     = carState->RadioRemote.data.owerflow_scan_positive;
                cmbNavMenuCounter                 = carState->RadioRemote.data.scroll_position;
                navMenuScroll                     = carState->RadioRemote.data.scroll_position;
            }

            message.data[0] = navMenuScroll;
            message.data[1] = navCommands.asByte;
            message.data[2] = phoneCommands.asByte;
            message.data[3] = cmbNavMenuCounter;
            message.data[4] = cmbCommands.asByte;

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {
            CAN_0A2_2004Struct packet;
            std::memcpy(&packet, message.data, sizeof(packet));

            carState->SteeringWheelRemote.data.NavMenuScroll         = packet.NavMenuScroll;

            carState->SteeringWheelRemote.data.voice_command         = packet.NavMenuCommands.data.voice_command;
            carState->SteeringWheelRemote.data.left_push             = packet.NavMenuCommands.data.left_push;
            carState->SteeringWheelRemote.data.mode_button           = packet.NavMenuCommands.data.mode_button;
            carState->SteeringWheelRemote.data.menu_button           = packet.NavMenuCommands.data.menu_button;
            carState->SteeringWheelRemote.data.escape_button         = packet.NavMenuCommands.data.escape_button;
            carState->SteeringWheelRemote.data.validation_button     = packet.NavMenuCommands.data.validation_button;
            carState->SteeringWheelRemote.data.scroll_underflow      = packet.NavMenuCommands.data.scroll_underflow;
            carState->SteeringWheelRemote.data.scroll_overflow       = packet.NavMenuCommands.data.scroll_overflow;

            carState->SteeringWheelRemote.data.telephone_command     = packet.PhoneCommands.data.telephone_command;

            carState->SteeringWheelRemote.data.volume_knob_underflow = packet.CmbCommands.data.volume_knob_underflow;
            carState->SteeringWheelRemote.data.volume_knob_overflow  = packet.CmbCommands.data.volume_knob_overflow;
            carState->SteeringWheelRemote.data.ac_recycling_command  = packet.CmbCommands.data.ac_recycling_command;
            carState->SteeringWheelRemote.data.cmb_escape_button     = packet.CmbCommands.data.cmb_escape_button;
            carState->SteeringWheelRemote.data.cmb_validation        = packet.CmbCommands.data.cmb_validation;
            carState->SteeringWheelRemote.data.cmb_underflow         = packet.CmbCommands.data.cmb_underflow;
            carState->SteeringWheelRemote.data.cmb_overflow          = packet.CmbCommands.data.cmb_overflow;

            carState->SteeringWheelRemote.data.CmbNavMenuCounter     = packet.CmbNavMenuCounter;

            carState->SteeringWheelRemote.data.VolumeCounter = 0; // Not present in this message
        }
};
#endif
