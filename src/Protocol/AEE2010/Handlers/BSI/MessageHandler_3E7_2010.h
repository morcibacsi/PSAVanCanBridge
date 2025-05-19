#pragma once

#ifndef _MessageHandler_3E7_2010_h
    #define _MessageHandler_3E7_2010_h

#include <inttypes.h>
#include <memory>
#include "../../../IMessageHandler.hpp"
#include "../../Structs/CAN_3E7_2010.h"

class MessageHandler_3E7_2010 : public IMessageHandler<MessageHandler_3E7_2010>
{
    private:
        BusMessage message
        {
            .id = 0x3E7,
            .data = {0},
            .dataLength = 5,
            .protocol = ProtocolType::AEE2010,
            .periodicityMs = 500,
            .offsetMs = 480,
            .isActive = true
        };
    public:
        static constexpr uint32_t MessageId = 0x3E7;

        BusMessage Generate(CarState* state)
        {
            CAN_3E7_2010Byte1Struct field1{};
            field1.data.maintenance_type_km   = state->IsMaintenanceDue;
            field1.data.wrench_icon           = state->WrenchIcon;
            field1.data.maintenance_sign_km   = state->MaintenanceSignKm;
            field1.data.maintenance_sign_time = state->MaintenanceSignTime;
            field1.data.km_blinking           = state->MaintenanceSignTimeBlinking || state->MaintenanceSignKmBlinking;

            message.data[0] = field1.asByte;
            message.data[1] = state->MaintenanceTime.data.leftByte;
            message.data[2] = state->MaintenanceTime.data.rightByte;
            message.data[3] = state->MaintenanceKilometers.data.leftByte;
            message.data[4] = state->MaintenanceKilometers.data.rightByte;

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {

        }
};
#endif
