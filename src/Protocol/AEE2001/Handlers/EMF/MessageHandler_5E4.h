#pragma once

#ifndef _MessageHandler_5E4_h
    #define _MessageHandler_5E4_h

#include <inttypes.h>
#include <memory>
#include <cstring>
#include <functional>

#include "../../Structs/VAN_5E4.h"
#include "../../../IMessageHandler.hpp"

const uint8_t VAN_COMFORT_STATE_IDLE = 0;
const uint8_t VAN_COMFORT_SEND_ALIVE = 1;
const uint8_t VAN_COMFORT_SEND_STANDBY = 2;

class MessageHandler_5E4 : public IMessageHandler<MessageHandler_5E4>
{
    uint64_t _tripButtonPressedSince = 0;
    uint8_t _prevTripButtonState = 0;
    uint8_t _prevIgnition = 0;
    uint8_t _vanComfortState = VAN_COMFORT_STATE_IDLE;

    uint8_t GetVanComfortState(CarState* carState)
    {
        uint8_t result = 0;

        if (carState->Ignition == 1)
        {
            result = 1;
        }
        else
        {
            if (_prevIgnition)
            {
                _vanComfortState = VAN_COMFORT_SEND_ALIVE;
            }
            switch (_vanComfortState)
            {
                case VAN_COMFORT_SEND_ALIVE:
                    result = 1;
                    _vanComfortState = VAN_COMFORT_SEND_STANDBY;
                    break;
                case VAN_COMFORT_SEND_STANDBY:
                    result = 0;
                    _vanComfortState = VAN_COMFORT_STATE_IDLE;
                    break;

                default:
                    result = 0;
                    _vanComfortState = VAN_COMFORT_STATE_IDLE;
                    break;
            }
        }

        _prevIgnition = carState->Ignition;

        return result;
    }

    public:
        static constexpr uint32_t MessageId = 0x5E4;

        MessageHandler_5E4(
        )
        {
        }

        BusMessage Generate(CarState* carState)
        {
            uint8_t resetTotals = 0;
            uint8_t resetCumulative = 0;

            if (carState->RightStickButtonPushed)
            {
                if (_prevTripButtonState == 0)
                {
                    _prevTripButtonState = 1;
                    _tripButtonPressedSince = carState->CurrenTime;
                }

                if (_tripButtonPressedSince != 0 && carState->CurrenTime - _tripButtonPressedSince > 3000)
                {
                    _tripButtonPressedSince = 0;
                    switch (carState->CurrentEmfMode)
                    {
                        case 2:
                            resetTotals = 1;
                            break;
                        case 4:
                            resetCumulative = 1;
                            break;
                        case 7:
                            resetTotals = 1;
                            resetCumulative = 1;
                            break;

                        default:
                            break;
                    }
                }
            }
            else
            {
                _prevTripButtonState = 0;
                _tripButtonPressedSince = 0;
            }

            VanEmfStatusByte0Struct field1{};
            field1.data.keep_alive_van_comfort = GetVanComfortState(carState);
            field1.data.cumulative_trip_reset = carState->Ignition && resetCumulative;
            field1.data.reset_course_totals = carState->Ignition && resetTotals;

            bool emulateDisplay = carState->EMULATE_DISPLAY_ON_SOURCE;
            bool reverseNotEngaged = (carState->IsReverseEngaged == 0);
            bool reverseEngagedButParkingAidIsNotVanBusType =
                (carState->IsReverseEngaged == 1 && carState->PARKING_AID_TYPE != 0x01);

            BusMessage msg;
            msg.id = 0x5E4;
            msg.protocol = ProtocolType::AEE2001;
            msg.type = MessageType::Normal;
            msg.ack = true;
            msg.periodicityMs = 500;
            msg.isActive = emulateDisplay && (reverseNotEngaged || reverseEngagedButParkingAidIsNotVanBusType);
            msg.data[0] = field1.asByte;
            msg.data[1] = 0x1E;
            msg.dataLength = 2;

            return msg;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {

        }
};
#endif
