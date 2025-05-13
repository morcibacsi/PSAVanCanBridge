#pragma once

#ifndef _MessageHandler_128_2010_h
    #define _MessageHandler_128_2010_h

#include <inttypes.h>
#include <memory>
#include "../../../IMessageHandler.hpp"
#include "../../Structs/CAN_128_2010.h"

class MessageHandler_128_2010 : public IMessageHandler
{
    private:
        BusMessage message
        {
            .id = 0x128,
            .data = {0},
            .dataLength = 8,
            .protocol = ProtocolType::AEE2010,
            .periodicityMs = 200,
            .offsetMs = 60,
            .isActive = true
        };
    public:
        BusMessage Generate(std::shared_ptr<CarState> state) override
        {
            Can2010CombineLightsByte1 field0{};
            field0.data.right_turn_indicator    = state->CarSignalLights.data.right_turn_indicator;
            field0.data.left_turn_indicator     = state->CarSignalLights.data.left_turn_indicator;
            field0.data.front_foglight          = state->CarSignalLights.data.front_foglight;
            field0.data.rear_foglight           = state->CarSignalLights.data.rear_foglight;
            field0.data.low_beam_on             = state->CarSignalLights.data.low_beam_on;
            field0.data.high_beam_on            = state->CarSignalLights.data.high_beam_on;
            field0.data.parking_light_indicator = state->CarSignalLights.data.parking_light_indicator;
            field0.data.drl                     = state->CarSignalLights.data.drl;

            /*
            field0.data.right_turn_indicator    = _dataBroker->RightTurnIndicator;
            field0.data.left_turn_indicator     = _dataBroker->LeftTurnIndicator;
            field0.data.front_foglight          = _dataBroker->FrontFog;
            field0.data.rear_foglight           = _dataBroker->RearFog;
            field0.data.low_beam_on             = _dataBroker->LowBeam;
            field0.data.high_beam_on            = _dataBroker->HighBeam;
            field0.data.parking_light_indicator = _dataBroker->SideLights;
            field0.data.drl                     = _dataBroker->DRL;
            */
        //
        //TODO
        ///*
            Can2010CombineLightsByte2 field1{};
            field1.data.display_blinking         = state->GearBlinking;
            field1.data.gear_position_in_driving = state->GearPositionInDriving;
            field1.data.gear_position            = state->GearPositionCmb;

            Can2010CombineLightsByte3 field2{};
            field2.data.auto_gearbox_selection     = state->GearBoxSelection;
            field2.data.recommended_gear_direction = state->RecommendedGearDirection;
            field2.data.auto_gearbox_mode          = state->GearBoxMode;
            field2.data.recommended_gear_blinking  = state->RecommendedGearBlinking;

        //*/
            Can2010CombineLightsByte4 field3{};
            field3.data.fse_inhibited                      = state->CarSignalLights.data.fse_inhibited;
            field3.data.handbrake_signal                   = state->CarSignalLights.data.handbrake_signal;
            field3.data.foot_on_break_indicator            = state->CarSignalLights.data.foot_on_break_indicator;
            field3.data.passenger_airbag_activated         = state->CarSignalLights.data.passenger_airbag_activated;
            field3.data.child_security_active              = state->CarSignalLights.data.child_security_active;
            field3.data.stop_light                         = state->CarSignalLights.data.stop_light;
            field3.data.service_indicator_exclamation      = state->CarSignalLights.data.service_indicator_exclamation;
            /*
            field3.data.fse_inhibited                      = _dataBroker->FSEDisabled;
            field3.data.handbrake_signal                   = _dataBroker->Handbrake;
            field3.data.foot_on_break_indicator            = _dataBroker->FootOnBrake;
            field3.data.passenger_airbag_activated         = _dataBroker->PassengerAirbagDeactivated;
            field3.data.child_security_active              = _dataBroker->ChildSafetyActivated;
            field3.data.stop_light                         = _dataBroker->StopOnCMB;
            field3.data.service_indicator_exclamation      = _dataBroker->ServiceOnCMB;
            */

            Can2010CombineLightsByte5 field4{};
            field4.data.suspension_status                  = state->CarSignalLights.data.suspension_status;
            field4.data.esp_in_progress                    = state->CarSignalLights.data.esp_in_progress;
            field4.data.esp_inactivated                    = state->CarSignalLights.data.esp_inactivated;
            field4.data.operation_indicator_light_blinking = state->CarSignalLights.data.operation_indicator_light_blinking;
            field4.data.operation_indicator_light_on       = state->CarSignalLights.data.operation_indicator_light_on;

            field4.data.door_open                          = state->DoorStatus.asByte > 0;
            field4.data.diesel_pre_heating                 = state->CarSignalLights.data.diesel_pre_heating;

            /*
            field4.data.suspension_status                  = _dataBroker->SuspensionStatus;
            field4.data.esp_in_progress                    = _dataBroker->EspInRegulation;
            field4.data.esp_inactivated                    = _dataBroker->EspDeactivated;
            field4.data.operation_indicator_light_blinking = _dataBroker->SpaceMeasureBlinking;
            field4.data.operation_indicator_light_on       = _dataBroker->SpaceMeasureOn;

            field4.data.door_open                          = _dataBroker->DoorStatusByte > 0;
            field4.data.diesel_pre_heating                 = _dataBroker->PreHeatingStatus;
            */
            Can2010CombineLightsByte6 field5{};
            field5.data.row1_rl_seatbelt_forgotten          = state->CarSignalLights.data.row1_rl_seatbelt_forgotten;
            field5.data.scr_indicator                       = state->CarSignalLights.data.scr_indicator;
            field5.data.passenger_seatbelt_warning_blinking = state->CarSignalLights.data.passenger_seatbelt_warning_blinking;
            field5.data.passenger_seatbelt_warning          = state->CarSignalLights.data.passenger_seatbelt_warning;
            field5.data.driver_seatbelt_warning             = state->CarSignalLights.data.driver_seatbelt_warning;
            field5.data.driver_seatbelt_warning_blinking    = state->CarSignalLights.data.driver_seatbelt_warning_blinking;
            field5.data.fuel_level_low                      = state->CarSignalLights.data.fuel_level_low;
            /*
            field5.data.row1_rl_seatbelt_forgotten          = _dataBroker->Row1RLSeatBelt;
            field5.data.scr_indicator                       = _dataBroker->SCRIndicator;
            field5.data.passenger_seatbelt_warning_blinking = _dataBroker->PassengerSeatbeltBlinking;
            field5.data.passenger_seatbelt_warning          = _dataBroker->PassengerSeatbeltWarning;
            field5.data.driver_seatbelt_warning             = _dataBroker->DriverSeatbeltWarning;
            field5.data.driver_seatbelt_warning_blinking    = _dataBroker->DriverSeatbeltWarningBlinking;
            field5.data.fuel_level_low                      = _dataBroker->FuelLow;
            */
            Can2010CombineLightsByte7 field6{};
            field6.data.activate_front_passenger_protection = state->CarSignalLights.data.activate_front_passenger_protection;
            field6.data.warning_light_active                = state->CarSignalLights.data.warning_light_active;
            field6.data.cmb_active                          = state->CarSignalLights.data.cmb_active;
            field6.data.row1_rr_seatbelt_forgotten_blinking = state->CarSignalLights.data.row1_rr_seatbelt_forgotten_blinking;
            field6.data.row1_rr_seatbelt_forgotten          = state->CarSignalLights.data.row1_rr_seatbelt_forgotten;
            field6.data.row1_rc_seatbelt_forgotten_blinking = state->CarSignalLights.data.row1_rc_seatbelt_forgotten_blinking;
            field6.data.row1_rc_seatbelt_forgotten          = state->CarSignalLights.data.row1_rc_seatbelt_forgotten;
            field6.data.row1_rl_seatbelt_forgotten_blinking = state->CarSignalLights.data.row1_rl_seatbelt_forgotten_blinking;

            /*
            field6.data.activate_front_passenger_protection = _dataBroker->ActivatePassengerProtection;
            field6.data.warning_light_active                = _dataBroker->WarningLight;
            field6.data.cmb_active                          = _dataBroker->OdometerEnabled;
            field6.data.row1_rr_seatbelt_forgotten_blinking = _dataBroker->Row1RRSeatBeltBlinking;
            field6.data.row1_rr_seatbelt_forgotten          = _dataBroker->Row1RRSeatBelt;
            field6.data.row1_rc_seatbelt_forgotten_blinking = _dataBroker->Row1RCSeatBeltBlinking;
            field6.data.row1_rc_seatbelt_forgotten          = _dataBroker->Row1RCSeatBelt;
            field6.data.row1_rl_seatbelt_forgotten_blinking = _dataBroker->Row1RLSeatBeltBlinking;
            */

            Can2010CombineLightsByte8 field7{};
            field7.data.change_personalization              = state->CarSignalLights.data.change_personalization;
            field7.data.change_color                        = state->CarSignalLights.data.change_color;
            field7.data.ready_lamp                          = state->CarSignalLights.data.ready_lamp;
            field7.data.minc_blinking                       = state->CarSignalLights.data.minc_blinking;
            field7.data.foot_on_clutch                      = state->CarSignalLights.data.foot_on_clutch;
            /*
            field7.data.change_personalization              = _dataBroker->ChangePersonalization;
            field7.data.change_color                        = _dataBroker->ChangeColor;
            field7.data.ready_lamp                          = _dataBroker->Ready;
            field7.data.minc_blinking                       = _dataBroker->MINCBlinking;
            field7.data.foot_on_clutch                      = _dataBroker->FootOnClutch;
            */

            //TODO
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
};
#endif
