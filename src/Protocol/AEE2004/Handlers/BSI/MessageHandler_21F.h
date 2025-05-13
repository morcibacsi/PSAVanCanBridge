#pragma once

#ifndef _MessageHandler_21F_2004_h
    #define _MessageHandler_21F_2004_h

#include <inttypes.h>
#include <memory>
#include <cstring>

#include "../../Structs/CAN_21F.h"
#include "../../../IMessageHandler.hpp"

class MessageHandler_21F : public IMessageHandler
{
    private:
        BusMessage message
        {
            .id = 0x21F,
            .data = { 0 },
            .dataLength = 3,
            .protocol = ProtocolType::AEE2004,
            .periodicityMs = 100,
            .offsetMs = 60,
            .isActive = true
        };

        std::function<void(ImmediateSignal)> _immediateSignalCallback;

    public:
        MessageHandler_21F(
            std::function<void(ImmediateSignal)> immediateSignalCallback
        )
        {
            _immediateSignalCallback = immediateSignalCallback;
        }

        BusMessage Generate(std::shared_ptr<CarState> state) override
        {
            CAN_21F_Byte1Struct field1{};
            field1.data.list                   = state->RadioRemote.data.list;
            field1.data.mode_phone             = state->RadioRemote.data.mode_phone;
            field1.data.volume_minus           = state->RadioRemote.data.volume_minus;
            field1.data.volume_plus            = state->RadioRemote.data.volume_plus;
            field1.data.owerflow_scan_negative = state->RadioRemote.data.owerflow_scan_negative;
            field1.data.owerflow_scan_positive = state->RadioRemote.data.owerflow_scan_positive;
            field1.data.seek_down              = state->RadioRemote.data.seek_down;
            field1.data.seek_up                = state->RadioRemote.data.seek_up;

            CAN_21F_Byte3Struct field3{};
            field3.data.command_valid = state->RadioRemote.data.command_valid;
            field3.data.list_minus    = state->RadioRemote.data.list_minus;
            field3.data.list_plus     = state->RadioRemote.data.list_plus;
            field3.data.source        = state->RadioRemote.data.source;

            message.data[0] = field1.asByte;
            message.data[1] = state->RadioRemote.data.scroll_position;
            message.data[2] = field3.asByte;

            return message;
        }

        void Parse(std::shared_ptr<CarState> carState, const BusMessage& message) override
        {
            constexpr std::size_t ExpectedPacketSize = sizeof(CAN_21F_2004_Struct);

            uint8_t packetSize = message.dataLength;
            if (packetSize != 2 && packetSize != ExpectedPacketSize)
            {
                return;
            }

            //CAN_21F_2004_Struct tmp;
            //std::memcpy(&tmp, message.data, packetSize);
            const auto* tmp = reinterpret_cast<const CAN_21F_2004_Struct*>(message.data);

            carState->RadioRemote.data.list                   = tmp->Command.data.list;
            carState->RadioRemote.data.mode_phone             = tmp->Command.data.mode_phone;
            carState->RadioRemote.data.owerflow_scan_negative = tmp->Command.data.owerflow_scan_negative;
            carState->RadioRemote.data.owerflow_scan_positive = tmp->Command.data.owerflow_scan_positive;
            carState->RadioRemote.data.seek_down              = tmp->Command.data.seek_down;
            carState->RadioRemote.data.seek_up                = tmp->Command.data.seek_up;
            carState->RadioRemote.data.volume_minus           = tmp->Command.data.volume_minus;
            carState->RadioRemote.data.volume_plus            = tmp->Command.data.volume_plus;

            carState->RadioRemote.data.scroll_position = tmp->ScrollPosition;

            if (packetSize > 2)
            {
                carState->RadioRemote.data.command_valid = tmp->Command3.data.command_valid;
                carState->RadioRemote.data.list_minus    = tmp->Command3.data.list_minus;
                carState->RadioRemote.data.list_plus     = tmp->Command3.data.list_plus;
                carState->RadioRemote.data.source        = tmp->Command3.data.source;
            }

            if (_immediateSignalCallback)
            {
                _immediateSignalCallback(ImmediateSignal::RadioRemote);
            }
        }
};
#endif
