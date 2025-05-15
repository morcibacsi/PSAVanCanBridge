#pragma once

#ifndef _MessageHandler_9C4_h
    #define _MessageHandler_9C4_h

#include <inttypes.h>
#include <memory>
#include <cstring>

#include "../../Structs/VAN_9C4.h"
#include "../../../AEE2004/Structs/CAN_10B.h"
#include "../../../IMessageHandler.hpp"

class MessageHandler_9C4 : public IMessageHandler
{
    ImmediateSignalCallback _immediateSignalCallback;

    public:
        MessageHandler_9C4(ImmediateSignalCallback immediateSignalCallback)
        {
            _immediateSignalCallback = immediateSignalCallback;
        }

        BusMessage Generate(CarState* state) override
        {
            BusMessage message;
            message.id = 0x9C4;
            message.periodicityMs = 500;
            message.offsetMs = 20;
            message.protocol = ProtocolType::AEE2001;

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message) override
        {
            constexpr std::size_t ExpectedPacketSize = sizeof(VanRadioRemoteStruct);

            if (message.dataLength != ExpectedPacketSize)
            {
                return;
            }

            VanRadioRemoteStruct vanPacket;
            std::memcpy(&vanPacket, message.data, ExpectedPacketSize);

            carState->RadioRemote.data.scroll_position = vanPacket.ScroolPosition;

            carState->RadioRemote.data.source          = vanPacket.ButtonStatus.data.source;
            carState->RadioRemote.data.volume_minus    = vanPacket.ButtonStatus.data.volume_minus;
            carState->RadioRemote.data.volume_plus     = vanPacket.ButtonStatus.data.volume_plus;
            carState->RadioRemote.data.seek_up         = vanPacket.ButtonStatus.data.seek_up;
            carState->RadioRemote.data.seek_down       = vanPacket.ButtonStatus.data.seek_down;

            carState->RadioRemote.data.owerflow_scan_positive = vanPacket.ButtonStatus.data.counter_overflow_positive;
            carState->RadioRemote.data.owerflow_scan_negative = vanPacket.ButtonStatus.data.counter_overflow_negative;

            if (_immediateSignalCallback)
            {
                _immediateSignalCallback(ImmediateSignal::RadioRemote);
            }
        }
};
#endif
