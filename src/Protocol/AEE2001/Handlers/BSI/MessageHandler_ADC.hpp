#pragma once

#ifndef _MessageHandler_ADC_h
    #define _MessageHandler_ADC_h

#include <cstdint>
#include <cstring>

#include "../../Structs/VAN_ADC.h"
#include "../../../../Helpers/IntUnions.h"
#include "../../../IMessageHandler.hpp"
#include "../../../FeedbackSignal.hpp"

class MessageHandler_ADC : public IMessageHandler<MessageHandler_ADC>
{
    FeedbackSignalCallback _feedbackSignalCallback;
    uint8_t _prevIgnition = 0;
    uint8_t _state = 0;

    private:
        uint8_t GetACDirection(uint8_t vanByte)
        {
            uint8_t result = 0x00;
            if (vanByte > 0x00)
            {
                result = 0x40; //CAN_AIRCON_DISPLAY_DIRECTION_UP;
            }
            if (vanByte > 0x1D)
            {
                result = 0x60; //CAN_AIRCON_DISPLAY_DIRECTION_UPDOWN;
            }
            if (vanByte > 0x2E)
            {
                result = 0x20; //CAN_AIRCON_DISPLAY_DIRECTION_DOWN;
            }
            if (vanByte > 0x3E)
            {
                result = 0x50; //CAN_AIRCON_DISPLAY_DIRECTION_FRONTDOWN;
            }
            if (vanByte > 0x60)
            {
                result = 0x30; //CAN_AIRCON_DISPLAY_DIRECTION_FRONT;
            }
            return result;
        }

    public:
        static constexpr uint32_t MessageId = 0xADC;

        MessageHandler_ADC()
        {
        }

        void SetFeedbackSignalCallback(FeedbackSignalCallback feedbackSignalCallback) { _feedbackSignalCallback = feedbackSignalCallback; }

        BusMessage Generate(CarState* carState)
        {
            BusMessage message;
            message.id = MessageId;
            message.periodicityMs = 500;
            message.offsetMs = 20;
            message.protocol = ProtocolType::AEE2001;
            message.type = MessageType::Query;
            message.isActive = false; //it is sent as a feedback message when A5C is received

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {
            if ((message.dataLength != 12 && message.dataLength != 22) || carState->QUERY_AC_STATUS != 1)
            {
                return;
            }

            if (message.data[2] == 0xC0)
            {
                VanAirConditionerDiagSensorStatusStruct packet;
                std::memcpy(&packet, message.data, sizeof(VanAirConditionerDiagSensorStatusStruct));

                UInt16 internalTemperature;
                internalTemperature.data.leftByte  = packet.InternalTemperature1;
                internalTemperature.data.rightByte = packet.InternalTemperature2;

                uint8_t internalTemperatureInCelsius = (internalTemperature.asUint16 * 0.1f) - 40;
                uint8_t internalTemperatureInCelsiusPsaFormat = (internalTemperatureInCelsius + 40) * 2; // to fit in 0-255 range
                carState->AirConditionerStatus.data.Temperature = internalTemperatureInCelsiusPsaFormat;

                return;
            }


            if (message.data[2] == 0xC1)
            {
                VanAirConditionerDiagActuatorStatusStruct packet;
                std::memcpy(&packet, message.data, sizeof(VanAirConditionerDiagActuatorStatusStruct));

                carState->AirConditionerStatus.data.Direction = GetACDirection(packet.DistributionStatus);

                return;
            }
        }
};
#endif
