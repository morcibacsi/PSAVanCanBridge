#pragma once

#ifndef _MessageHandler_3A7_h
    #define _MessageHandler_3A7_h

#include <cstdint>
#include <cstring>

#include "../../Structs/CAN_3A7_2004.h"
#include "../../../IMessageHandler.hpp"

class MessageHandler_3A7 : public IMessageHandler<MessageHandler_3A7>
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
        static constexpr uint32_t MessageId = 0x3A7;

        BusMessage Generate(CarState* carState)
        {
            Can3A7Byte1Struct field1{};
            field1.data.maintenance_due = carState->IsMaintenanceDue;
            field1.data.wrench_with_km = carState->WrenchIcon;

            Can3A7Byte2Struct field2{};
            field2.data.maintenance_sign_km = carState->MaintenanceSignKm;
            field2.data.km_blinking = carState->MaintenanceSignKmBlinking;

            Can3A7Byte3Struct field3{};
            field3.data.maintenance_sign_time = carState->MaintenanceSignTime;
            field3.data.time_blinking = carState->MaintenanceSignTimeBlinking;

            message.data[0] = field1.asByte;
            message.data[1] = field2.asByte;
            message.data[2] = field3.asByte;
            message.data[3] = carState->MaintenanceKilometers.data.leftByte;
            message.data[4] = carState->MaintenanceKilometers.data.rightByte;
            message.data[5] = carState->MaintenanceTime.data.leftByte;
            message.data[6] = carState->MaintenanceTime.data.rightByte;
            message.data[7] = 0x05;

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {
            Can3A7Struct packet;
            std::memcpy(&packet, message.data, sizeof(packet));

            carState->IsMaintenanceDue          = packet.MaintenanceType.data.maintenance_due;
            carState->WrenchIcon                = packet.MaintenanceType.data.wrench_with_km;
            carState->MaintenanceSignKm         = packet.KmMaintenanceInfo.data.maintenance_sign_km;
            carState->MaintenanceSignTime       = packet.TemporaryMaintenanceInfo.data.maintenance_sign_time;
            carState->MaintenanceSignKmBlinking = packet.KmMaintenanceInfo.data.km_blinking;
            carState->MaintenanceSignTime       = packet.TemporaryMaintenanceInfo.data.time_blinking;

            carState->MaintenanceKilometers.data.leftByte  = packet.MaintenanceKmByte1;
            carState->MaintenanceKilometers.data.rightByte = packet.MaintenanceKmByte2;

            carState->MaintenanceTime.data.leftByte  = packet.NumberOfDaysBeforeMaintenanceByte1;
            carState->MaintenanceTime.data.rightByte = packet.NumberOfDaysBeforeMaintenanceByte2;
        }
};
#endif
