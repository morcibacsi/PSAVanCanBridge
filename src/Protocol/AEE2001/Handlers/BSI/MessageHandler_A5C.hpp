#pragma once

#ifndef _MessageHandler_A5C_h
    #define _MessageHandler_A5C_h

#include <cstdint>
#include <cstring>

#include "../../Structs/VAN_8C4.h"
#include "../../../IMessageHandler.hpp"
#include "../../../FeedbackSignal.hpp"

const uint8_t STATE_DEFAULT = 0;
const uint8_t STATE_INIT = 1;
const uint8_t STATE_GET_SENSOR_DATA = 2;
const uint8_t STATE_GET_ACTUATOR_DATA = 3;

class MessageHandler_A5C : public IMessageHandler<MessageHandler_A5C>
{

    FeedbackSignalCallback _feedbackSignalCallback;
    uint8_t _prevIgnition = 0;
    uint8_t _state = STATE_DEFAULT;

    public:
        static constexpr uint32_t MessageId = 0xA5C;

        MessageHandler_A5C()
        {
        }

        void SetFeedbackSignalCallback(FeedbackSignalCallback feedbackSignalCallback) { _feedbackSignalCallback = feedbackSignalCallback; }

        BusMessage Generate(CarState* carState)
        {
            if (_prevIgnition == 0 && carState->Ignition == 1)
            {
                _state = STATE_INIT;
            }

            _prevIgnition = carState->Ignition;

            BusMessage message;
            message.id = MessageId;
            message.periodicityMs = 300;
            message.offsetMs = 40;
            message.protocol = ProtocolType::AEE2001;
            message.type = MessageType::Normal;
            message.ack = true;
            message.dataLength = 2;
            message.isActive = carState->QUERY_AC_STATUS == 1 && carState->IsReverseEngaged == 0 && carState->RightStickButtonPushed == 0;

            switch (_state)
            {
                case STATE_INIT:
                    message.data[0] = 0x21;
                    message.data[1] = 0x80;
                    break;
                case STATE_GET_SENSOR_DATA:
                    // get sensor data (temperature)
                    message.data[0] = 0x21;
                    message.data[1] = 0xC0;
                    break;
                case STATE_GET_ACTUATOR_DATA:
                    // get actuator data (airflow direction)
                    message.data[0] = 0x21;
                    message.data[1] = 0xC1;
                    break;

                default:
                    message.isActive = false;
                    break;
            }

            if (message.isActive == false)
            {
                _state = STATE_INIT;
            }

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {
            if (message.dataLength != 2)
            {
                return;
            }

            // init
            if (message.data[0] == 0x21 && message.data[1] == 0x80)
            {
                _state = STATE_GET_ACTUATOR_DATA;
                _feedbackSignalCallback(FeedbackSignal::QueryAirConData);
                return;
            }

            // get sensor data (temperature)
            if (message.data[0] == 0x21 && message.data[1] == 0xC0)
            {
                _state = STATE_GET_ACTUATOR_DATA;
                _feedbackSignalCallback(FeedbackSignal::QueryAirConData);
                return;
            }

            // get actuator data (airflow direction)
            if (message.data[0] == 0x21 && message.data[1] == 0xC1)
            {
                //_state = STATE_GET_SENSOR_DATA; // This state is not used in this handler, but could be used for future expansion
                _state = STATE_GET_ACTUATOR_DATA;
                _feedbackSignalCallback(FeedbackSignal::QueryAirConData);
                return;
            }
        }
};
#endif
