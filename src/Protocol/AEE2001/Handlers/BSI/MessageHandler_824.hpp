#pragma once

#ifndef _MessageHandler_824_h
    #define _MessageHandler_824_h

#include <cstdint>
#include <cstring>

#include "../../../IMessageHandler.hpp"
#include "../../../ImmediateSignal.hpp"

class MessageHandler_824 : public IMessageHandler<MessageHandler_824>
{
    ImmediateSignalCallback _immediateSignalCallback;

    public:
        static constexpr uint32_t MessageId = 0x824;

    MessageHandler_824()
    {
    }

    void SetImmediateSignalCallback(ImmediateSignalCallback immediateSignalCallback) { _immediateSignalCallback = immediateSignalCallback; }

    BusMessage Generate(CarState* carState)
        {
            BusMessage message;
            message.id = MessageId;
            message.periodicityMs = 50;
            message.offsetMs = 20;
            message.protocol = ProtocolType::AEE2001;
            message.type = MessageType::Normal;
            message.isActive = false;
            message.ack = false;

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
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
