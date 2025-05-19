#pragma once

#ifndef _MessageHandler_1A1_h
    #define _MessageHandler_1A1_h

#include <inttypes.h>
#include <memory>
#include <cstring>

#include "../../../IMessageHandler.hpp"

class MessageHandler_1A1 : public IMessageHandler<MessageHandler_1A1>
{
    private:
        BusMessage message
        {
            .id = 0x1A1,
            .data = {0},
            .dataLength = 8,
            .protocol = ProtocolType::AEE2004,
            .periodicityMs = 200,
            .offsetMs = 110,
            .isActive = true
        };

        ImmediateSignalCallback _immediateSignalCallback;
    public:
        static constexpr uint32_t MessageId = 0x1A1;

        MessageHandler_1A1()
        {
        }

        void SetImmediateSignalCallback(ImmediateSignalCallback immediateSignalCallback) { _immediateSignalCallback = immediateSignalCallback; }

        BusMessage Generate(CarState* state)
        {
            message.data[0] = state->DisplayMessage.data.Field1;
            message.data[1] = state->DisplayMessage.data.Field2;
            message.data[2] = state->DisplayMessage.data.Field3;
            message.data[3] = state->DisplayMessage.data.Field4;
            message.data[4] = state->DisplayMessage.data.Field5;
            message.data[5] = state->DisplayMessage.data.Field6;
            message.data[6] = state->DisplayMessage.data.Field7;
            message.data[7] = state->DisplayMessage.data.Field8;

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {
            if (message.dataLength < 8)
            {
                return;
            }

            carState->DisplayMessage.data.Field1 = message.data[0];
            carState->DisplayMessage.data.Field2 = message.data[1];
            carState->DisplayMessage.data.Field3 = message.data[2];
            carState->DisplayMessage.data.Field4 = message.data[3];
            carState->DisplayMessage.data.Field5 = message.data[4];
            carState->DisplayMessage.data.Field6 = message.data[5];
            carState->DisplayMessage.data.Field7 = message.data[6];
            carState->DisplayMessage.data.Field8 = message.data[7];

            if (_immediateSignalCallback)
            {
                _immediateSignalCallback(ImmediateSignal::PopupMessage);
            }
        }
};
#endif
