#pragma once

#ifndef _MessageHandler_21F_2004_h
    #define _MessageHandler_21F_2004_h

#include <cstdint>
#include <cstring>

#include "../../Structs/CAN_21F_2004.h"
#include "../../../IMessageHandler.hpp"
#include "../../../ImmediateSignal.hpp"

class MessageHandler_21F : public IMessageHandler<MessageHandler_21F>
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

        ImmediateSignalCallback _immediateSignalCallback = nullptr;

    public:
        static constexpr uint32_t MessageId = 0x21F;

        MessageHandler_21F()
        {
        }

        void SetImmediateSignalCallback(ImmediateSignalCallback immediateSignalCallback) { _immediateSignalCallback = immediateSignalCallback; }

        BusMessage Generate(CarState* carState)
        {
            bool emulateSteeringWheelControls = carState->EMULATE_STEERING_WHEEL_CONTROLS_WITH_STALK;

            CAN_21F_Byte1Struct field1{};
            field1.data.list                   = carState->RadioRemote.data.list;
            field1.data.mode_phone             = carState->RadioRemote.data.mode_phone;
            field1.data.volume_minus           = carState->RadioRemote.data.volume_minus;
            field1.data.volume_plus            = carState->RadioRemote.data.volume_plus;
            field1.data.owerflow_scan_negative = emulateSteeringWheelControls ? 0 : carState->RadioRemote.data.owerflow_scan_negative;
            field1.data.owerflow_scan_positive = emulateSteeringWheelControls ? 0 : carState->RadioRemote.data.owerflow_scan_positive;
            field1.data.seek_down              = carState->RadioRemote.data.seek_down;
            field1.data.seek_up                = carState->RadioRemote.data.seek_up;

            CAN_21F_Byte3Struct field3{};
            field3.data.command_valid = carState->RadioRemote.data.command_valid;
            field3.data.list_minus    = carState->RadioRemote.data.list_minus;
            field3.data.list_plus     = carState->RadioRemote.data.list_plus;
            field3.data.source        = carState->RadioRemote.data.source;

            if (carState->REPLACE_REMOTE_MODE_BTN_WITH_SRC)
            {
                field1.data.mode_phone = field3.data.source;
                field3.data.source = 0;
            }

            message.data[0] = field1.asByte;
            message.data[1] = emulateSteeringWheelControls ? 0x00 : carState->RadioRemote.data.scroll_position;
            message.data[2] = field3.asByte;

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {
            constexpr std::size_t ExpectedPacketSize = sizeof(CAN_21F_2004_Struct);

            uint8_t packetSize = message.dataLength;
            if (packetSize != 2 && packetSize != ExpectedPacketSize)
            {
                return;
            }

            CAN_21F_2004_Struct packet{};
            std::memcpy(&packet, message.data, packetSize);

            CarRadioRemoteStruct input = carState->RadioRemote;
            input.data.list                   = packet.Command.data.list;
            input.data.mode_phone             = packet.Command.data.mode_phone;
            input.data.owerflow_scan_negative = packet.Command.data.owerflow_scan_negative;
            input.data.owerflow_scan_positive = packet.Command.data.owerflow_scan_positive;
            input.data.seek_down              = packet.Command.data.seek_down;
            input.data.seek_up                = packet.Command.data.seek_up;
            input.data.volume_minus           = packet.Command.data.volume_minus;
            input.data.volume_plus            = packet.Command.data.volume_plus;
            input.data.scroll_position        = packet.ScrollPosition;

            if (packetSize > 2)
            {
                input.data.command_valid = packet.Command3.data.command_valid;
                input.data.list_minus    = packet.Command3.data.list_minus;
                input.data.list_plus     = packet.Command3.data.list_plus;
                input.data.source        = packet.Command3.data.source;
            }

            const auto result = carState->RadioRemoteVolumeControlState.Process(input, carState->CurrenTime);
            carState->RadioRemote = result.remote;

            if (_immediateSignalCallback != nullptr)
            {
                if (result.pulse != RadioRemoteVolumeControl::Pulse::None)
                {
                    carState->RadioRemote.data.volume_minus =
                        result.pulse == RadioRemoteVolumeControl::Pulse::VolumeMinus;
                    carState->RadioRemote.data.volume_plus =
                        result.pulse == RadioRemoteVolumeControl::Pulse::VolumePlus;
                    _immediateSignalCallback(ImmediateSignal::RadioRemote);
                    carState->RadioRemote.data.volume_minus = 0;
                    carState->RadioRemote.data.volume_plus = 0;
                }
                _immediateSignalCallback(ImmediateSignal::RadioRemote);
            }
        }
};
#endif
