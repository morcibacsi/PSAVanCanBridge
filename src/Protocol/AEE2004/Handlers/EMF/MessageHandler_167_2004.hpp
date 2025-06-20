#pragma once

#ifndef _MessageHandler_167_h
    #define _MessageHandler_167_h

#include <cstdint>

#include "../../../IMessageHandler.hpp"
#include "../../Structs/CAN_167_2004.h"

class MessageHandler_167 : public IMessageHandler<MessageHandler_167>
{
    private:
        BusMessage message
        {
            .id = 0x167,
            .data = {0},
            .dataLength = 8,
            .protocol = ProtocolType::AEE2004,
            .periodicityMs = 100,
            .offsetMs = 65,
            //TODO
            .isActive = false
        };
    public:
        static constexpr uint32_t MessageId = 0x167;

        BusMessage Generate(CarState* carState)
        {
            Can2004_167Byte1 byte1{};
            byte1.data.trip_data_on_odometer = ODOMETER_TRIP_INFO_NOT_MANAGED_BY_EMF;

            //TODO
            //if (_config->NO_EMF_C_INSTALLED)
            {
                //only send when setting is enabled
            }

            message.data[0] = byte1.asByte;
            //*
            message.data[1] = 0x00;
            message.data[2] = 0x00;
            message.data[3] = 0x00;
            message.data[4] = 0x00;
            message.data[5] = 0x00;
            message.data[6] = 0x00;
            message.data[7] = 0x00;
            //*/

            return message;
        }

        void Parse(CarState* carState, const BusMessage& msg)
        {
            CAN2004_167Struct packet;
            std::memcpy(&packet, message.data, sizeof(packet));

            carState->CurrentEmfMode = packet.EMFRequest.data.trip_data_on_odometer;
        }
};
#endif
