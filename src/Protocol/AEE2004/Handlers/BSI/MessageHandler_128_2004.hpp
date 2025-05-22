#pragma once

#ifndef _MessageHandler_128_2004_h
    #define _MessageHandler_128_2004_h

#include <cstdint>
#include <cstring>

#include "../../../IMessageHandler.hpp"
#include "../../Structs/CAN_128_2004.h"
#include "../../../ImmediateSignal.hpp"

class MessageHandler_128 : public IMessageHandler<MessageHandler_128>
{
    private:
        BusMessage message
        {
            .id = 0x128,
            .data = { 0 },
            .dataLength = 8,
            .protocol = ProtocolType::AEE2004,
            .periodicityMs = 200,
            .offsetMs = 60,
            .isActive = true
        };

        ImmediateSignalCallback _immediateSignalCallback;

    public:
        static constexpr uint32_t MessageId = 0x128;

        MessageHandler_128()
        {
        }

        void SetImmediateSignalCallback(ImmediateSignalCallback immediateSignalCallback) { _immediateSignalCallback = immediateSignalCallback; }

        BusMessage Generate(CarState* carState)
        {
            Can2004CombineLightsByte1 field0{};
            field0.data.fuel_level_low             = carState->CarSignalLights.data.fuel_level_low;
            field0.data.driver_seatbelt_warning    = carState->CarSignalLights.data.driver_seatbelt_warning;
            field0.data.passenger_seatbelt_warning = carState->CarSignalLights.data.passenger_seatbelt_warning;
            field0.data.handbrake_signal           = carState->CarSignalLights.data.handbrake_signal;
            //field0.data.air_bag_fault              = carState->CarIndicatorLights.data.a
            field0.data.diesel_pre_heating         = carState->CarSignalLights.data.diesel_pre_heating;

            Can2004CombineLightsByte2 field1{};
            field1.data.abs_active = carState->ABSInProgess;
            field1.data.stop_light = carState->CarSignalLights.data.stop_light;
            //field1.data.service_indicator_exclamation = 0;

            Can2004CombineLightsByte3 field2{};
            field2.data.esp_inactivated = carState->CarSignalLights.data.esp_inactivated;
            field2.data.esp_in_progress = carState->CarSignalLights.data.esp_in_progress;

            Can2004CombineLightsByte4 field3{};
            field3.data.operation_indicator_light_on        = carState->CarSignalLights.data.operation_indicator_light_on;
            field3.data.operation_indicator_light_blinking  = carState->CarSignalLights.data.operation_indicator_light_blinking;
            field3.data.foot_on_break_indicator             = carState->CarSignalLights.data.foot_on_break_indicator;
            field3.data.rear_seatbelt_warning_blinking      = carState->CarSignalLights.data.row1_rc_seatbelt_forgotten || carState->CarSignalLights.data.row1_rr_seatbelt_forgotten || carState->CarSignalLights.data.row1_rl_seatbelt_forgotten;
            field3.data.autopark_light                      = carState->CarSignalLights.data.parking_light_indicator;
            field3.data.passenger_seatbelt_warning_blinking = carState->CarSignalLights.data.passenger_seatbelt_warning_blinking;

            Can2004CombineLightsByte5 field4{};
            field4.data.right_turn_indicator    = carState->CarSignalLights.data.right_turn_indicator;
            field4.data.left_turn_indicator     = carState->CarSignalLights.data.left_turn_indicator;
            field4.data.front_foglight          = carState->CarSignalLights.data.front_foglight;
            field4.data.rear_foglight           = carState->CarSignalLights.data.rear_foglight;
            field4.data.low_beam_on             = carState->CarSignalLights.data.low_beam_on;
            field4.data.high_beam_on            = carState->CarSignalLights.data.high_beam_on;
            field4.data.parking_light_indicator = carState->CarSignalLights.data.parking_light_indicator;

            Can2004CombineLightsByte6 field5{};
            field5.data.cmb_active = carState->OdometerEnabled;

            Can2004CombineLightsByte7 field6{};
            field6.data.gear_position_cmb = carState->GearPositionCmb;

            Can2004CombineLightsByte8 field7{};
            field7.data.auto_gearbox_mode      = carState->GearBoxMode;
            field7.data.auto_gearbox_selection = carState->GearBoxSelection;

            message.data[0] = field0.asByte;
            message.data[1] = field1.asByte;
            message.data[2] = field2.asByte;
            message.data[3] = field3.asByte;
            message.data[4] = field4.asByte;
            message.data[5] = field5.asByte;
            message.data[6] = field6.asByte;
            message.data[7] = field7.asByte;

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {
            CMB2004_128Struct packet;
            std::memcpy(&packet, message.data, sizeof(packet));

            carState->CarSignalLights.data.handbrake_signal                    = packet.Indicator1.data.handbrake_signal;
            carState->CarSignalLights.data.passenger_airbag_activated          = packet.Indicator1.data.passenger_airbag_deactivated;
            carState->CarSignalLights.data.diesel_pre_heating                  = packet.Indicator1.data.diesel_pre_heating;
            carState->CarSignalLights.data.passenger_seatbelt_warning          = packet.Indicator1.data.passenger_seatbelt_warning;
            carState->CarSignalLights.data.driver_seatbelt_warning             = packet.Indicator1.data.driver_seatbelt_warning;
            carState->CarSignalLights.data.fuel_level_low                      = packet.Indicator1.data.fuel_level_low;
            //carState->CarSignalLights.data.fuel                                = packet.Indicator1.data.fuel_circuit_neutral;
            //carState->CarSignalLights.data.fuel_level_low                      = packet.Indicator1.data.service_icon_blink;

            carState->CarSignalLights.data.activate_front_passenger_protection = packet.Indicator2.data.activate_front_passenger_protection;
            carState->CarSignalLights.data.stop_light                          = packet.Indicator2.data.stop_light;
            carState->CarSignalLights.data.service_indicator_exclamation       = packet.Indicator2.data.service_indicator_exclamation;

            carState->CarSignalLights.data.warning_light_active                = packet.Indicator3.data.warning_light_active;
            carState->CarSignalLights.data.change_personalization              = packet.Indicator3.data.change_personalization;
            carState->CarSignalLights.data.change_color                        = packet.Indicator3.data.change_color;
            carState->CarSignalLights.data.ready_lamp                          = packet.Indicator3.data.ready_lamp;
            carState->CarSignalLights.data.suspension_status                   = packet.Indicator3.data.suspension_status;
            carState->CarSignalLights.data.esp_in_progress                     = packet.Indicator3.data.esp_in_progress;
            carState->CarSignalLights.data.esp_inactivated                     = packet.Indicator3.data.esp_inactivated;
            carState->CarSignalLights.data.child_security_active               = packet.Indicator3.data.child_security_active;

            carState->CarSignalLights.data.passenger_seatbelt_warning_blinking = packet.Indicator4.data.passenger_seatbelt_warning_blinking;
            carState->CarSignalLights.data.driver_seatbelt_warning_blinking    = packet.Indicator4.data.driver_seatbelt_warning_blinking;
            carState->CarSignalLights.data.foot_on_break_indicator             = packet.Indicator4.data.foot_on_break_indicator;
            carState->CarSignalLights.data.operation_indicator_light_blinking  = packet.Indicator4.data.operation_indicator_light_blinking;
            carState->CarSignalLights.data.operation_indicator_light_on        = packet.Indicator4.data.operation_indicator_light_on;
            //carState->CarSignalLights.data.operation_indicator_light_on        = packet.Indicator4.data.autopark_light;
            carState->CarSignalLights.data.operation_indicator_light_on        = packet.Indicator4.data.rear_seatbelt_warning_blinking;

            carState->CarSignalLights.data.drl                     = packet.Indicator5.data.drl;
            carState->CarSignalLights.data.left_turn_indicator     = packet.Indicator5.data.left_turn_indicator;
            carState->CarSignalLights.data.right_turn_indicator    = packet.Indicator5.data.right_turn_indicator;
            carState->CarSignalLights.data.rear_foglight           = packet.Indicator5.data.rear_foglight;
            carState->CarSignalLights.data.front_foglight          = packet.Indicator5.data.front_foglight;
            carState->CarSignalLights.data.high_beam_on            = packet.Indicator5.data.high_beam_on;
            carState->CarSignalLights.data.low_beam_on             = packet.Indicator5.data.low_beam_on;
            carState->CarSignalLights.data.parking_light_indicator = packet.Indicator5.data.parking_light_indicator;

            carState->CarSignalLights.data.fse_inhibited                       = packet.Indicator6.data.fse_inhibited;
            carState->CarSignalLights.data.row1_rl_seatbelt_forgotten          = packet.Indicator6.data.row1_rl_seatbelt_forgotten;
            carState->CarSignalLights.data.row1_rl_seatbelt_forgotten_blinking = packet.Indicator6.data.row1_rl_seatbelt_forgotten_blinking;
            carState->CarSignalLights.data.row1_rr_seatbelt_forgotten          = packet.Indicator6.data.row1_rr_seatbelt_forgotten;
            carState->CarSignalLights.data.row1_rr_seatbelt_forgotten_blinking = packet.Indicator6.data.row1_rr_seatbelt_forgotten_blinking;
            carState->CarSignalLights.data.row1_rc_seatbelt_forgotten          = packet.Indicator6.data.row1_rc_seatbelt_forgotten;
            carState->CarSignalLights.data.row1_rc_seatbelt_forgotten_blinking = packet.Indicator6.data.row1_rc_seatbelt_forgotten_blinking;
            carState->CarSignalLights.data.cmb_active                          = packet.Indicator6.data.cmb_active;

            carState->GearBlinking             = packet.Indicator7.data.display_blinking;
            carState->GearPositionInDriving    = packet.Indicator7.data.gear_position_drive;
            carState->GearPositionCmb          = packet.Indicator7.data.gear_position_cmb;
            carState->GearBoxSelection         = packet.Indicator8.data.auto_gearbox_selection;
            carState->RecommendedGearDirection = packet.Indicator8.data.recommended_gear_direction;
            carState->GearBoxMode              = packet.Indicator8.data.auto_gearbox_mode;
            carState->RecommendedGearBlinking  = packet.Indicator8.data.recommended_gear_blinking;

            if (_immediateSignalCallback)
            {
                _immediateSignalCallback(ImmediateSignal::CmbIndicatorLightsChanged);
                _immediateSignalCallback(ImmediateSignal::CmbSignalLightsChanged);
            }
        }
};
#endif
