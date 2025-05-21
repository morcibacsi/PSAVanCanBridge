#pragma once

#ifndef _MessageHandler_227_2004_h
    #define _MessageHandler_227_2004_h

#include <inttypes.h>
#include <memory>
#include <cstring>

#include "../../Structs/CAN_227_2004.h"
#include "../../../IMessageHandler.hpp"

class MessageHandler_227 : public IMessageHandler<MessageHandler_227>
{
    private:
        BusMessage message
        {
            .id = 0x227,
            .data = { 0 },
            .dataLength = 5,
            .protocol = ProtocolType::AEE2004,
            .periodicityMs = 500,
            .offsetMs = 25,
            .isActive = true
        };

    public:
        static constexpr uint32_t MessageId = 0x227;

        BusMessage Generate(CarState* carState)
        {
            CAN_227_2004_Byte1Struct field1{};

            field1.data.aas_led         = carState->CarLedStatusByte1.data.aas_led;
            field1.data.amvar_sport_led = carState->CarLedStatusByte1.data.amvar_sport_led;
            field1.data.child_lock_led  = carState->CarLedStatusByte1.data.child_lock_led;
            field1.data.esp_led         = carState->CarLedStatusByte1.data.esp_led;

            CAN_227_2004_Byte2Struct field2{};
            field2.data.ac_on_led                    = carState->CarLedStatusByte2.data.ac_on_led;
            field2.data.rear_window_heating_led      = carState->CarLedStatusByte2.data.rear_window_heating_led;
            field2.data.secondary_electric_brake_led = carState->CarLedStatusByte2.data.secondary_electric_brake_led;
            field2.data.stl_led                      = carState->CarLedStatusByte2.data.stl_led;

            CAN_227_2004_Byte3Struct field3{};
            field3.data.ac_airflow_type           = carState->CarLedStatusByte3.data.ac_airflow_type;
            field3.data.blind_spot_monitoring_led = carState->CarLedStatusByte3.data.blind_spot_monitoring_led;
            field3.data.fuel_info                 = carState->CarLedStatusByte3.data.fuel_info;
            field3.data.stop_start_led            = carState->CarLedStatusByte3.data.stop_start_led;

            CAN_227_2004_Byte4Struct field4{};
            field4.data.ac_recycling              = carState->CarLedStatusByte4.data.ac_airflow_recycling;

            message.data[0] = field1.asByte;
            message.data[1] = field2.asByte;
            message.data[2] = field3.asByte;
            message.data[3] = field4.asByte;
            message.data[4] = 0x00;

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {
            CAN_227_2004_Struct packet;
            std::memcpy(&packet, message.data, static_cast<std::size_t>(sizeof(packet)));

            carState->CarLedStatusByte1.data.aas_led                      = packet.Field1.data.aas_led;
            carState->CarLedStatusByte1.data.amvar_sport_led              = packet.Field1.data.amvar_sport_led;
            carState->CarLedStatusByte1.data.child_lock_led               = packet.Field1.data.child_lock_led;
            carState->CarLedStatusByte1.data.esp_led                      = packet.Field1.data.esp_led;

            carState->CarLedStatusByte2.data.ac_on_led                    = packet.Field2.data.ac_on_led;
            carState->CarLedStatusByte2.data.rear_window_heating_led      = packet.Field2.data.rear_window_heating_led;
            carState->CarLedStatusByte2.data.secondary_electric_brake_led = packet.Field2.data.secondary_electric_brake_led;
            carState->CarLedStatusByte2.data.stl_led                      = packet.Field2.data.stl_led;

            carState->CarLedStatusByte3.data.ac_airflow_type              = packet.Field3.data.ac_airflow_type;
            carState->CarLedStatusByte3.data.blind_spot_monitoring_led    = packet.Field3.data.blind_spot_monitoring_led;
            carState->CarLedStatusByte3.data.fuel_info                    = packet.Field3.data.fuel_info;
            carState->CarLedStatusByte3.data.stop_start_led               = packet.Field3.data.stop_start_led;

            carState->CarLedStatusByte4.data.ac_airflow_recycling         = packet.Field3.data.ac_airflow_type;
            //carState->CarLedStatusByte4.data. = packet.Field3.data.;
        }
};
#endif
