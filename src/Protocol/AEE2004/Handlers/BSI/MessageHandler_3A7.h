#pragma once

#ifndef _MessageHandler_3A7_h
    #define _MessageHandler_3A7_h

#include <inttypes.h>
#include <memory>
#include <cstring>

#include "../../Structs/CAN_3A7.h"
#include "../../../IMessageHandler.hpp"

class MessageHandler_3A7 : public IMessageHandler
{
    private:
        BusMessage message
        {
            .id = 0x3A7,
            .data = { 0 },
            .dataLength = 8,
            .protocol = ProtocolType::AEE2004,
            .periodicityMs = 500,
            .offsetMs = 240,
            .isActive = true
        };
    public:
        BusMessage Generate(CarState* state) override
        {
            Can3A7Byte1Struct field1{};
            field1.data.maintenance_due = state->IsMaintenanceDue;
            field1.data.wrench_with_km = state->WrenchIcon;

            Can3A7Byte2Struct field2{};
            field2.data.maintenance_sign_km = state->MaintenanceSignKm;
            field2.data.km_blinking = state->MaintenanceSignKmBlinking;

            Can3A7Byte3Struct field3{};
            field3.data.maintenance_sign_time = state->MaintenanceSignTime;
            field3.data.time_blinking = state->MaintenanceSignTimeBlinking;

            message.data[0] = field1.asByte;
            message.data[1] = field2.asByte;
            message.data[2] = field3.asByte;
            message.data[3] = state->MaintenanceKilometers.data.leftByte;
            message.data[4] = state->MaintenanceKilometers.data.rightByte;
            message.data[5] = state->MaintenanceTime.data.leftByte;
            message.data[6] = state->MaintenanceTime.data.rightByte;
            message.data[7] = 0x05;

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message) override
        {
            //Can3A7Struct tmp;
            //std::memcpy(&tmp, message.data, static_cast<std::size_t>(sizeof(tmp)));
            const auto* tmp = reinterpret_cast<const Can3A7Struct*>(message.data);

            carState->IsMaintenanceDue          = tmp->MaintenanceType.data.maintenance_due;
            carState->WrenchIcon                = tmp->MaintenanceType.data.wrench_with_km;
            carState->MaintenanceSignKm         = tmp->KmMaintenanceInfo.data.maintenance_sign_km;
            carState->MaintenanceSignTime       = tmp->TemporaryMaintenanceInfo.data.maintenance_sign_time;
            carState->MaintenanceSignKmBlinking = tmp->KmMaintenanceInfo.data.km_blinking;
            carState->MaintenanceSignTime       = tmp->TemporaryMaintenanceInfo.data.time_blinking;

            carState->MaintenanceKilometers.data.leftByte  = tmp->MaintenanceKmByte1;
            carState->MaintenanceKilometers.data.rightByte = tmp->MaintenanceKmByte2;

            carState->MaintenanceTime.data.leftByte  = tmp->NumberOfDaysBeforeMaintenanceByte1;
            carState->MaintenanceTime.data.rightByte = tmp->NumberOfDaysBeforeMaintenanceByte2;
        }
};
#endif
