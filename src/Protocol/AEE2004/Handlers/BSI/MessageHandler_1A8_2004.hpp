#pragma once

#ifndef _MessageHandler_1A8_h
    #define _MessageHandler_1A8_h

#include <cstdint>
#include <cstring>

#include "../../Structs/CAN_1A8_2004.h"
#include "../../../IMessageHandler.hpp"
#include "../../../ImmediateSignal.hpp"

class MessageHandler_1A8 : public IMessageHandler<MessageHandler_1A8>
{
    private:
        BusMessage message
        {
            .id = 0x1A8,
            .data = { 0 },
            .dataLength = 8,
            .protocol = ProtocolType::AEE2004,
            .periodicityMs = 200,
            .offsetMs = 90,
            .isActive = true
        };

        ImmediateSignalCallback _immediateSignalCallback;

    public:
        static constexpr uint32_t MessageId = 0x1A8;

        MessageHandler_1A8()
        {
        }

        void SetImmediateSignalCallback(ImmediateSignalCallback immediateSignalCallback) { _immediateSignalCallback = immediateSignalCallback; }

        BusMessage Generate(CarState* carState)
        {
            CAN_1A8_2004_Byte1Struct field1{};
            field1.data.unit_of_speed               = carState->CruiseControlSpeedUnit;
            field1.data.activate_function           = carState->CruiseControlActivateFunction;
            field1.data.selected_function           = carState->CruiseControlSelectedFunction;
            field1.data.status_of_selected_function = carState->CruiseControlStatusOfSelectedFunction;

            message.data[0] = field1.asByte;
            message.data[1] = carState->CruiseControlSpeed.data.leftByte;
            message.data[2] = carState->CruiseControlSpeed.data.rightByte;
            message.data[3] = 0x00;
            message.data[4] = 0x00;
            message.data[5] = carState->TripOnCMB.data.leftByte;
            message.data[6] = carState->TripOnCMB.data.middleByte;
            message.data[7] = carState->TripOnCMB.data.rightByte;

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {
            CAN_1A8_2004Struct packet;
            std::memcpy(&packet, message.data, sizeof(packet));

            carState->CruiseControlSpeed.data.leftByte      = packet.CruiseControlSpeed1;
            carState->CruiseControlSpeed.data.rightByte     = packet.CruiseControlSpeed2;
            carState->CruiseControlActivateFunction         = packet.Field1.data.activate_function;
            carState->CruiseControlSelectedFunction         = packet.Field1.data.selected_function;
            carState->CruiseControlStatusOfSelectedFunction = packet.Field1.data.status_of_selected_function;
            carState->TripOnCMB.data.leftByte               = packet.TripOnCMB1;
            carState->TripOnCMB.data.middleByte             = packet.TripOnCMB2;
            carState->TripOnCMB.data.rightByte              = packet.TripOnCMB3;

            if (_immediateSignalCallback)
            {
                _immediateSignalCallback(ImmediateSignal::CruiseControl);
            }
        }
};
#endif
