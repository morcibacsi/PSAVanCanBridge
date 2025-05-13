#pragma once

#ifndef _MessageHandler_824_h
    #define _MessageHandler_824_h

#include <inttypes.h>
#include <memory>
#include <cstring>

#include "../../../IMessageHandler.hpp"

class MessageHandler_824 : public IMessageHandler
{
    std::function<void(ImmediateSignal)> _immediateSignalCallback;

    public:
    MessageHandler_824(
        std::function<void(ImmediateSignal)> immediateSignalCallback
    )
    {
        _immediateSignalCallback = std::move(immediateSignalCallback);
    }

    BusMessage Generate(std::shared_ptr<CarState> state) override
        {
            BusMessage message;
            message.id = 0x824;
            message.periodicityMs = 50;
            message.offsetMs = 20;
            message.protocol = ProtocolType::AEE2001;

            return message;
        }

        void Parse(std::shared_ptr<CarState> carState, const BusMessage& message) override
        {
            constexpr std::size_t ExpectedPacketSize = 7;

            if (message.dataLength != ExpectedPacketSize)
            {
                return;
            }

            carState->Rpm.data.leftByte    = message.data[0];
            carState->Rpm.data.rightByte   = message.data[1];

            carState->Speed.data.leftByte  = message.data[2];
            carState->Speed.data.rightByte = message.data[3];
            carState->SpeedInKmh = carState->Speed.asUint16 * 0.01;

            carState->DistanceForCMB.data.leftByte  = message.data[4];
            carState->DistanceForCMB.data.rightByte = message.data[5];

            carState->ConsumptionForCMB = message.data[6];

            if (_immediateSignalCallback)
            {
                _immediateSignalCallback(ImmediateSignal::SpeedAndRpm);
            }
        }
};
#endif
