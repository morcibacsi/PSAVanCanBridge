#pragma once

#ifndef _MessageHandler_9C4_h
    #define _MessageHandler_9C4_h

#include <cstdint>
#include <cstring>

#include "../../Structs/VAN_9C4.h"
#include "../../../AEE2004/Structs/CAN_10B_2004.h"
#include "../../../IMessageHandler.hpp"
#include "../../../ImmediateSignal.hpp"

class MessageHandler_9C4 : public IMessageHandler<MessageHandler_9C4>
{
    ImmediateSignalCallback _immediateSignalCallback = nullptr;

    public:
        static constexpr uint32_t MessageId = 0x9C4;

        MessageHandler_9C4()
        {
        }

        void SetImmediateSignalCallback(ImmediateSignalCallback immediateSignalCallback) { _immediateSignalCallback = immediateSignalCallback; }

        BusMessage Generate(CarState* carState)
        {
            BusMessage message{};
            message.id = MessageId;
            message.periodicityMs = 500;
            message.offsetMs = 20;
            message.protocol = ProtocolType::AEE2001;
            message.type = MessageType::Normal;
            message.isActive = false;
            message.ack = true;

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {
            constexpr std::size_t ExpectedPacketSize = sizeof(VanRadioRemoteStruct);

            if (message.dataLength != ExpectedPacketSize)
            {
                return;
            }

            VanRadioRemoteStruct packet;
            std::memcpy(&packet, message.data, ExpectedPacketSize);

            CarRadioRemoteStruct input = carState->RadioRemote;
            input.data.scroll_position = packet.ScroolPosition;
            input.data.source          = packet.ButtonStatus.data.source;
            input.data.volume_minus    = packet.ButtonStatus.data.volume_minus;
            input.data.volume_plus     = packet.ButtonStatus.data.volume_plus;
            input.data.seek_up         = packet.ButtonStatus.data.seek_up;
            input.data.seek_down       = packet.ButtonStatus.data.seek_down;
            input.data.owerflow_scan_positive = packet.ButtonStatus.data.counter_overflow_positive;
            input.data.owerflow_scan_negative = packet.ButtonStatus.data.counter_overflow_negative;

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
