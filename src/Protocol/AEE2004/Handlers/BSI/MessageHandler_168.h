#pragma once

#ifndef _MessageHandler_168_h
    #define _MessageHandler_168_h

#include <inttypes.h>
#include <memory>
#include <cstring>
#include "../../../IMessageHandler.hpp"
#include "../../Structs/CAN_168.h"

class MessageHandler_168 : public IMessageHandler<MessageHandler_168>
{
    private:
        BusMessage message
        {
            .id = 0x168,
            .data = { 0 },
            .dataLength = 5,
            .protocol = ProtocolType::AEE2004,
            .periodicityMs = 200,
            .offsetMs = 10,
            .isActive = true
        };

        ImmediateSignalCallback _immediateSignalCallback;

    public:
        static constexpr uint32_t MessageId = 0x168;

        MessageHandler_168()
        {
        }

        void SetImmediateSignalCallback(ImmediateSignalCallback immediateSignalCallback) { _immediateSignalCallback = immediateSignalCallback; }

        BusMessage Generate(CarState* state)
        {
            CanDash3Byte1Struct field1{};
            field1.data.auto_gearbox_alert       = state->CarIndicatorLights.data.gearbox_fault;
            field1.data.break_fluid_alert        = state->CarIndicatorLights.data.break_fluid_alert;
            field1.data.coolant_level_alert      = state->CarIndicatorLights.data.coolant_level_alert;
            field1.data.coolant_temp_max         = state->CarIndicatorLights.data.coolant_temp_max;
            field1.data.oil_level_alert          = state->CarIndicatorLights.data.oil_level_alert;
            field1.data.oil_pressure_alert       = state->CarIndicatorLights.data.oil_pressure_alert;
            //TODO
            //field1.data.oil_temp_max             = state->CarIndicatorLights.data.oil_temp_max;

            CanDash3Byte2Struct field2{};
            //TODO
            //field2.data.diesel_additive_alert    = state->CarIndicatorLights.data.diesel_additive_alert;
            field2.data.fap_clogged              = state->CarIndicatorLights.data.fap_clogged;
            field2.data.tyre_punctured           = state->CarIndicatorLights.data.tyre_punctured;
            field2.data.tyre_pressure_low        = state->CarIndicatorLights.data.tyre_pressure_low;

            CanDash3Byte3Struct field3{};
            field3.data.foot_on_clutch                      = state->CarSignalLights.data.foot_on_clutch;
            field3.data.row2_rr_seatbelt_forgotten          = state->CarIndicatorLights.data.row2_rr_seatbelt_forgot;
            field3.data.row2_rr_seatbelt_forgotten_blinking = state->CarIndicatorLights.data.row2_rr_seatbelt_blink;
            field3.data.row2_rl_seatbelt_forgotten          = state->CarIndicatorLights.data.row2_rl_seatbelt_forgot;
            field3.data.row2_rl_seatbelt_forgotten_blinking = state->CarIndicatorLights.data.row2_rl_seatbelt_blink;
            //TODO
            //field3.data.row2_rc_seatbelt_forgotten = state->CarIndicatorLights.data.r;

            CanDash3Byte4Struct field4{};
            field4.data.abs_fault                = state->CarIndicatorLights.data.abs_fault;
            field4.data.brake_pad_fault          = state->CarIndicatorLights.data.brake_pad_fault;
            field4.data.esp_fault                = state->CarIndicatorLights.data.esp_fault;
            field4.data.mil                      = state->CarIndicatorLights.data.mil;
            field4.data.gearbox_fault            = state->CarIndicatorLights.data.gearbox_fault;
            field4.data.water_in_diesel          = state->CarIndicatorLights.data.water_in_diesel;
            field4.data.serious_suspension_fault = state->CarIndicatorLights.data.serious_suspension_fault;

            CanDash3Byte5Struct field5{};
            field5.data.antipollution_fault      = state->CarIndicatorLights.data.antipollution_fault;
            field5.data.battery_charge_fault     = state->CarIndicatorLights.data.battery_charge_fault;
            //TODO
            //field5.data.diesel_additive_fault    = state->CarIndicatorLights.data.diesel_additive_fault;
            //field5.data.driver_airbag            = state->CarIndicatorLights.data.driver_airbag

            message.data[0] = field1.asByte;
            message.data[1] = field2.asByte;
            message.data[2] = field3.asByte;
            message.data[3] = field4.asByte;
            message.data[4] = field5.asByte;

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {
            //CanDash3Struct tmp;
            //std::memcpy(&tmp, message.data, static_cast<std::size_t>(sizeof(tmp)));
            const auto* tmp = reinterpret_cast<const CanDash3Struct*>(message.data);

            //TODO
            carState->CarIndicatorLights.data.gearbox_fault       = tmp->Field1.data.auto_gearbox_alert;
            carState->CarIndicatorLights.data.break_fluid_alert   = tmp->Field1.data.break_fluid_alert;
            carState->CarIndicatorLights.data.coolant_level_alert = tmp->Field1.data.coolant_level_alert;
            carState->CarIndicatorLights.data.coolant_temp_max    = tmp->Field1.data.coolant_temp_max;
            carState->CarIndicatorLights.data.dsg_fault           = tmp->Field1.data.dsg_fault;
            carState->CarIndicatorLights.data.oil_level_alert     = tmp->Field1.data.oil_level_alert;
            carState->CarIndicatorLights.data.oil_pressure_alert  = tmp->Field1.data.oil_pressure_alert;
            //carState->CarIndicatorLights.data.oi = tmp.Field1.data.oil_temp_max;

            carState->CarIndicatorLights.data.max_rpm_2           = tmp->Field2.data.max_rpm_2;
            carState->CarIndicatorLights.data.max_rpm_1           = tmp->Field2.data.max_rpm_1;
            carState->CarIndicatorLights.data.auto_wiping_active  = tmp->Field2.data.auto_wiping_active;
            carState->CarIndicatorLights.data.fap_clogged         = tmp->Field2.data.fap_clogged;
            carState->CarIndicatorLights.data.tyre_punctured      = tmp->Field2.data.tyre_punctured;
            carState->CarIndicatorLights.data.tyre_pressure_low   = tmp->Field2.data.tyre_pressure_low;
            //carState->CarIndicatorLights.data. = tmp.Field2.data.diesel_additive_alert;
            //carState->CarIndicatorLights.data.abs_fault = tmp.Field2.data.minc_blinking;

            carState->CarSignalLights.data.foot_on_clutch                  = tmp->Field3.data.foot_on_clutch;
            carState->CarIndicatorLights.data.row2_rr_seatbelt_forgot      = tmp->Field3.data.row2_rr_seatbelt_forgotten;
            carState->CarIndicatorLights.data.row2_rr_seatbelt_blink       = tmp->Field3.data.row2_rr_seatbelt_forgotten_blinking;
            carState->CarIndicatorLights.data.row2_rl_seatbelt_forgot      = tmp->Field3.data.row2_rl_seatbelt_forgotten;
            carState->CarIndicatorLights.data.row2_rl_seatbelt_blink       = tmp->Field3.data.row2_rl_seatbelt_forgotten_blinking;
            //carState->CarIndicatorLights.data. = tmp.Field3.data.row2_rc_seatbelt_forgotten;
            //carState->CarIndicatorLights.data. = tmp.Field3.data.row2_rc_seatbelt_forgotten_blinking;

            carState->CarIndicatorLights.data.water_in_diesel          = tmp->Field4.data.water_in_diesel;
            carState->CarIndicatorLights.data.serious_suspension_fault = tmp->Field4.data.serious_suspension_fault;
            carState->CarIndicatorLights.data.serious_ref_ehb_fault    = tmp->Field4.data.serious_ref_ehb_fault;
            carState->CarIndicatorLights.data.mil                      = tmp->Field4.data.mil;
            carState->CarIndicatorLights.data.brake_pad_fault          = tmp->Field4.data.brake_pad_fault;
            carState->CarIndicatorLights.data.gearbox_fault            = tmp->Field4.data.gearbox_fault;
            carState->CarIndicatorLights.data.esp_fault                = tmp->Field4.data.esp_fault;
            carState->CarIndicatorLights.data.abs_fault                = tmp->Field4.data.abs_fault;

            carState->CarSignalLights.data.scr_indicator              = tmp->Field5.data.scr_indicator;
            carState->CarIndicatorLights.data.generator_fault         = tmp->Field5.data.generator_fault;
            carState->CarIndicatorLights.data.battery_charge_fault    = tmp->Field5.data.battery_charge_fault;

            carState->CarIndicatorLights.data.curve_code_fault        = tmp->Field6.data.curve_code_fault;
            carState->CarIndicatorLights.data.caar_lamp_status        = tmp->Field6.data.caar_lamp_status;
            carState->CarIndicatorLights.data.gearbox_position        = tmp->Field6.data.gear_position_in_driving;

            carState->CarIndicatorLights.data.fse_tightening_fault    = tmp->Field7.data.fse_tightening_fault;
            carState->CarIndicatorLights.data.fse_system_fault        = tmp->Field7.data.fse_system_fault;
            carState->CarIndicatorLights.data.stt_lamp_status         = tmp->Field7.data.stt_lamp_status;
            carState->CarIndicatorLights.data.power_steering_fault    = tmp->Field7.data.power_steering_fault;


            if (_immediateSignalCallback)
            {
                _immediateSignalCallback(ImmediateSignal::CmbIndicatorLightsChanged);
                _immediateSignalCallback(ImmediateSignal::CmbSignalLightsChanged);
            }
        }
};
#endif
