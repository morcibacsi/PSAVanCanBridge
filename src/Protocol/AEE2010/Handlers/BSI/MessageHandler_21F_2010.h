#pragma once

#ifndef _MessageHandler_21F_2010_h
    #define _MessageHandler_21F_2010_h

#include <inttypes.h>
#include <memory>
#include "../../../IMessageHandler.hpp"
#include "../../Structs/CAN_21F_2010.h"

class MessageHandler_21F_2010 : public IMessageHandler
{
    private:
        BusMessage message
        {
            .id = 0x21F,
            .data = {0},
            .dataLength = 3,
            .protocol = ProtocolType::AEE2010,
            .periodicityMs = 100,
            .offsetMs = 50,
            .isActive = true
        };
    public:
        BusMessage Generate(CarState* state) override
        {
            CAN_21F_2010_Byte1Struct field1{};
            field1.data.list                   = state->RadioRemote.data.list;
            field1.data.mode_phone             = state->RadioRemote.data.mode_phone;
            field1.data.owerflow_scan_negative = state->RadioRemote.data.owerflow_scan_negative;
            field1.data.owerflow_scan_positive = state->RadioRemote.data.owerflow_scan_positive;
            field1.data.seek_down              = state->RadioRemote.data.seek_down;
            field1.data.seek_up                = state->RadioRemote.data.seek_up;
            field1.data.volume_minus           = state->RadioRemote.data.volume_minus;
            field1.data.volume_plus            = state->RadioRemote.data.volume_plus;

            CAN_21F_2010_Byte3Struct field3{};
            field3.data.list_minus_vci            = state->RadioRemote.data.list_minus;
            field3.data.list_plus_vci             = state->RadioRemote.data.list_plus;
            field3.data.radio_command_validation  = state->RadioRemote.data.command_valid;
            field3.data.source                    = state->RadioRemote.data.source;
            field3.data.reco_voc_vci              = state->RadioRemote.data.unused5;

            if (state->REPLACE_REMOTE_MODE_BTN_WITH_SRC)
            {
                field3.data.source = field1.data.mode_phone;
                field1.data.mode_phone = 0;
            }

            message.data[0] = field1.asByte;
            message.data[1] = state->RadioRemote.data.scroll_position;
            message.data[2] = field3.asByte;

            return message;
        }
};
#endif
