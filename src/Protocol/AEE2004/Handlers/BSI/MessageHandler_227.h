#pragma once

#ifndef _MessageHandler_227_2004_h
    #define _MessageHandler_227_2004_h

#include <inttypes.h>
#include <memory>
#include <cstring>

#include "../../Structs/CAN_227_2004.h"
#include "../../../IMessageHandler.hpp"

class MessageHandler_227 : public IMessageHandler
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
        BusMessage Generate(std::shared_ptr<CarState> state) override
        {
            CAN_227_2004_Byte1Struct field1{};

            field1.data.aas_led = state->CarLedStatusByte1.data.aas_led;
            field1.data.amvar_sport_led = state->CarLedStatusByte1.data.amvar_sport_led;
            field1.data.child_lock_led = state->CarLedStatusByte1.data.child_lock_led;
            field1.data.esp_led = state->CarLedStatusByte1.data.esp_led;

            CAN_227_2004_Byte2Struct field2{};
            field2.data.ac_on_led = state->CarLedStatusByte2.data.ac_on_led;
            field2.data.rear_window_heating_led = state->CarLedStatusByte2.data.rear_window_heating_led;
            field2.data.secondary_electric_brake_led = state->CarLedStatusByte2.data.secondary_electric_brake_led;
            field2.data.stl_led = state->CarLedStatusByte2.data.stl_led;

            CAN_227_2004_Byte3Struct field3{};
            field3.data.ac_airflow_type = state->CarLedStatusByte3.data.ac_airflow_type;
            field3.data.blind_spot_monitoring_led = state->CarLedStatusByte3.data.blind_spot_monitoring_led;
            field3.data.fuel_info = state->CarLedStatusByte3.data.fuel_info;
            field3.data.stop_start_led = state->CarLedStatusByte3.data.stop_start_led;

            CAN_227_2004_Byte4Struct field4{};
            field4.data.ac_recycling = state->CarLedStatusByte4.data.ac_airflow_recycling;

            message.data[0] = field1.asByte;
            message.data[1] = field2.asByte;
            message.data[2] = field3.asByte;
            message.data[3] = field4.asByte;
            message.data[4] = 0x00;

            return message;
        }

        void Parse(std::shared_ptr<CarState> carState, const BusMessage& message) override
        {
            //CAN_227_2004_Struct tmp;
            //std::memcpy(&tmp, message.data, static_cast<std::size_t>(sizeof(tmp)));
            const auto* tmp = reinterpret_cast<const CAN_227_2004_Struct*>(message.data);

            carState->CarLedStatusByte1.data.aas_led         = tmp->Field1.data.aas_led;
            carState->CarLedStatusByte1.data.amvar_sport_led = tmp->Field1.data.amvar_sport_led;
            carState->CarLedStatusByte1.data.child_lock_led  = tmp->Field1.data.child_lock_led;
            carState->CarLedStatusByte1.data.esp_led         = tmp->Field1.data.esp_led;

            carState->CarLedStatusByte2.data.ac_on_led                    = tmp->Field2.data.ac_on_led;
            carState->CarLedStatusByte2.data.rear_window_heating_led      = tmp->Field2.data.rear_window_heating_led;
            carState->CarLedStatusByte2.data.secondary_electric_brake_led = tmp->Field2.data.secondary_electric_brake_led;
            carState->CarLedStatusByte2.data.stl_led                      = tmp->Field2.data.stl_led;

            carState->CarLedStatusByte3.data.ac_airflow_type              = tmp->Field3.data.ac_airflow_type;
            carState->CarLedStatusByte3.data.blind_spot_monitoring_led    = tmp->Field3.data.blind_spot_monitoring_led;
            carState->CarLedStatusByte3.data.fuel_info                    = tmp->Field3.data.fuel_info;
            carState->CarLedStatusByte3.data.stop_start_led               = tmp->Field3.data.stop_start_led;

            carState->CarLedStatusByte4.data.ac_airflow_recycling         = tmp->Field3.data.ac_airflow_type;
            //carState->CarLedStatusByte4.data. = tmp.Field3.data.;

            //_canMessageHandlerContainer2010->SetData(0x227);
        }
};
#endif
