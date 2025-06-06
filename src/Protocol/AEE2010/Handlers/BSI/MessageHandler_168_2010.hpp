#pragma once

#ifndef _MessageHandler_168_2010_h
    #define _MessageHandler_168_2010_h

#include <cstdint>

#include "../../../IMessageHandler.hpp"
#include "../../Structs/CAN_168_2010.h"

class MessageHandler_168_2010 : public IMessageHandler<MessageHandler_168_2010>
{
    private:
        BusMessage message
        {
            .id = 0x168,
            .data = {0},
            .dataLength = 7,
            .protocol = ProtocolType::AEE2010,
            .periodicityMs = 200,
            .offsetMs = 10,
            .isActive = true
        };
    public:
        static constexpr uint32_t MessageId = 0x168;

        BusMessage Generate(CarState* carState)
        {
            CanDash3Byte1_2010_Struct field1{};
            field1.data.coolant_temp_max = carState->CarIndicatorLights.data.coolant_temp_max;
            field1.data.number_of_gears = carState->CarIndicatorLights.data.number_of_gears;
            field1.data.coolant_level_alert = carState->CarIndicatorLights.data.coolant_level_alert;
            field1.data.oil_level_alert = carState->CarIndicatorLights.data.oil_level_alert;
            field1.data.oil_pressure_alert = carState->CarIndicatorLights.data.oil_pressure_alert;
            field1.data.break_fluid_alert = carState->CarIndicatorLights.data.break_fluid_alert;
            field1.data.engine_cold = carState->CarIndicatorLights.data.engine_cold;
            field1.data.dsg_fault = carState->CarIndicatorLights.data.dsg_fault;

            CanDash3Byte2_2010_Struct field2{};
            field2.data.antipollution_fault   = carState->CarIndicatorLights.data.antipollution_fault;
            field2.data.auto_wiping_active    = carState->CarIndicatorLights.data.auto_wiping_active;
            field2.data.max_rpm_1             = carState->CarIndicatorLights.data.max_rpm_1;
            field2.data.max_rpm_2             = carState->CarIndicatorLights.data.max_rpm_2;
            field2.data.tyre_punctured        = carState->CarIndicatorLights.data.tyre_punctured;
            field2.data.fap_clogged           = carState->CarIndicatorLights.data.fap_clogged;
            field2.data.tyre_pressure_low     = carState->CarIndicatorLights.data.tyre_pressure_low;
            /*
            field2.data.max_rpm_2           = _dataBroker->EngineSpeedThreshold2;
            field2.data.max_rpm_1           = _dataBroker->EngineSpeedThreshold1;
            field2.data.auto_wiping_active  = _dataBroker->AutoWipingActive;
            field2.data.fap_clogged         = _dataBroker->FapClogged;
            field2.data.flat_tyre_alert     = _dataBroker->FlatTyreAlert;
            field2.data.tyre_pressure_alert = _dataBroker->TyrePressureAlert;
            */
            CanDash3Byte3_2010_Struct field3{};

            field3.data.generator_fault          = carState->CarIndicatorLights.data.generator_fault;
            field3.data.battery_charge_fault     = carState->CarIndicatorLights.data.battery_charge_fault;
            field3.data.serious_suspension_fault = carState->CarIndicatorLights.data.serious_suspension_fault;
            field3.data.serious_ref_ehb_fault    = carState->CarIndicatorLights.data.serious_ref_ehb_fault;
            /*
            field3.data.generator_fault          = _dataBroker->GeneratorFault;
            field3.data.battery_charge_fault     = _dataBroker->BatteryChargeFault;
            field3.data.serious_suspension_fault = _dataBroker->SeriousSuspensionFault;
            field3.data.serious_ref_ehb_fault    = _dataBroker->SeriousREFFault;
            */
            CanDash3Byte4_2010_Struct field4{};
            field4.data.mil                  = carState->CarIndicatorLights.data.mil;
            field4.data.brake_pad_fault      = carState->CarIndicatorLights.data.brake_pad_fault;
            field4.data.gearbox_fault        = carState->CarIndicatorLights.data.gearbox_fault;
            field4.data.esp_fault            = carState->CarIndicatorLights.data.esp_fault;
            field4.data.abs_fault            = carState->CarIndicatorLights.data.abs_fault;
            field4.data.fse_tightening_fault = carState->CarIndicatorLights.data.fse_tightening_fault;
            field4.data.fse_system_fault     = carState->CarIndicatorLights.data.fse_system_fault;
            /*
            field4.data.mil                  = _dataBroker->Mil;
            field4.data.brake_pad_fault      = _dataBroker->BrakePadFault;
            field4.data.gearbox_fault        = _dataBroker->AutoGearboxFault;
            field4.data.esp_fault            = _dataBroker->EspFault;
            field4.data.abs_fault            = _dataBroker->AbsFault;
            field4.data.fse_tightening_fault = _dataBroker->FSETighteningFault;
            field4.data.fse_system_fault     = _dataBroker->FSESystemFault;
            */
            CanDash3Byte5_2010_Struct field5{};
            field5.data.stt_lamp_status      = carState->CarIndicatorLights.data.stt_lamp_status;
            field5.data.power_steering_fault = carState->CarIndicatorLights.data.power_steering_fault;
            field5.data.caar_lamp_status     = carState->CarIndicatorLights.data.caar_lamp_status;
            field5.data.curve_code_fault     = carState->CarIndicatorLights.data.curve_code_fault;
            field5.data.water_in_diesel      = carState->CarIndicatorLights.data.water_in_diesel;
            /*
            field5.data.stt_lamp_status      = _dataBroker->STTLampStatus;
            field5.data.power_steering_fault = _dataBroker->PowerSteeringFault;
            field5.data.caar_lamp_status     = _dataBroker->CAARLampStatus;
            field5.data.curve_code_fault     = _dataBroker->CurveCodeFault;
            field5.data.water_in_diesel      = _dataBroker->WaterInDiesel;
            */
            CanDash3Byte7_2010_Struct field7{};
            field7.data.gearbox_position = carState->CarIndicatorLights.data.gearbox_position;
            field7.data.authorize_vth    = carState->CarIndicatorLights.data.authorize_vth;
            /*
            field7.data.gearbox_position = _dataBroker->GearPosition;
            field7.data.authorize_vth    = _dataBroker->EnableVTH;
            */

            message.data[0] = field1.asByte;
            message.data[1] = field2.asByte;
            message.data[2] = field3.asByte;
            message.data[3] = field4.asByte;
            message.data[4] = field5.asByte;
            message.data[5] = 0x00;
            message.data[6] = field7.asByte;

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {

        }
};
#endif
