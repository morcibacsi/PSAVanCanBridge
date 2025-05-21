#pragma once

#ifndef _MessageHandler_228_2010_h
    #define _MessageHandler_228_2010_h

#include <inttypes.h>
#include <memory>
#include "../../../IMessageHandler.hpp"
#include "../../Structs/CAN_228_2010.h"

/*
    Cruise control related data
*/
class MessageHandler_228_2010 : public IMessageHandler<MessageHandler_228_2010>
{
    private:
        BusMessage message
        {
            .id = 0x228,
            .data = { 0 },
            .dataLength = 8,
            .protocol = ProtocolType::AEE2010,
            .periodicityMs = 1000,
            .offsetMs = 530,
            .isActive = true
        };
    public:
        static constexpr uint32_t MessageId = 0x228;

        BusMessage Generate(CarState* carState)
        {
            CAN_228_2010_Byte3Struct byte2{};
            byte2.data.activate_function           = carState->CruiseControlActivateFunction;
            byte2.data.selected_function           = carState->CruiseControlSelectedFunction;
            byte2.data.status_of_selected_function = carState->CruiseControlStatusOfSelectedFunction;
            byte2.data.setting_status = SETTING_STATUS_NO_ADJUSTMENT_2010;
            byte2.data.target_present = 1;
            //byte2.data. = 1;

            message.data[0] = carState->CruiseControlSpeed.data.leftByte;
            message.data[1] = carState->CruiseControlSpeed.data.rightByte;
            message.data[2] = byte2.asByte;
            message.data[3] = 0x80;
            message.data[4] = 0x14;
            message.data[5] = 0x7F;
            message.data[6] = 0xFF;
            message.data[7] = 0x98;

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {

        }
};
#endif
