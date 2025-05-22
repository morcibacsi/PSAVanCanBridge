#pragma once

#ifndef _MessageHandler_21F_2010_h
    #define _MessageHandler_21F_2010_h

#include <cstdint>

#include "../../../IMessageHandler.hpp"
#include "../../Structs/CAN_21F_2010.h"

class MessageHandler_21F_2010 : public IMessageHandler<MessageHandler_21F_2010>
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
        static constexpr uint32_t MessageId = 0x21F;

        BusMessage Generate(CarState* carState)
        {
            CAN_21F_2010_Byte1Struct field1{};
            field1.data.list                   = carState->RadioRemote.data.list;
            field1.data.mode_phone             = carState->RadioRemote.data.mode_phone;
            field1.data.owerflow_scan_negative = carState->RadioRemote.data.owerflow_scan_negative;
            field1.data.owerflow_scan_positive = carState->RadioRemote.data.owerflow_scan_positive;
            field1.data.seek_down              = carState->RadioRemote.data.seek_down;
            field1.data.seek_up                = carState->RadioRemote.data.seek_up;
            field1.data.volume_minus           = carState->RadioRemote.data.volume_minus;
            field1.data.volume_plus            = carState->RadioRemote.data.volume_plus;

            CAN_21F_2010_Byte3Struct field3{};
            field3.data.list_minus_vci            = carState->RadioRemote.data.list_minus;
            field3.data.list_plus_vci             = carState->RadioRemote.data.list_plus;
            field3.data.radio_command_validation  = carState->RadioRemote.data.command_valid;
            field3.data.source                    = carState->RadioRemote.data.source;
            field3.data.reco_voc_vci              = carState->RadioRemote.data.unused5;

            if (carState->REPLACE_REMOTE_MODE_BTN_WITH_SRC)
            {
                field3.data.source = field1.data.mode_phone;
                field1.data.mode_phone = 0;
            }

            message.data[0] = field1.asByte;
            message.data[1] = carState->RadioRemote.data.scroll_position;
            message.data[2] = field3.asByte;

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {

        }
};
#endif
