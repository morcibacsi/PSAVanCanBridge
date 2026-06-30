#pragma once

#ifndef _MessageHandler_120_2010_h
    #define _MessageHandler_120_2010_h

#include <cstdint>

#include "../../../IMessageHandler.hpp"
#include "../../../AEE2004/Structs/CAN_120_2004.h"

class MessageHandler_120_2010 : public IMessageHandler<MessageHandler_120_2010>
{
    private:
        uint64_t lastMessageTime = 0;
        uint8_t blockNumber = 1;

        BusMessage message
        {
            .id = 0x120,
            .data = {0},
            .dataLength = 8,
            .protocol = ProtocolType::AEE2010,
            .periodicityMs = 1000,
            .offsetMs = 390,
            .isActive = true
        };
    public:
        static constexpr uint32_t MessageId = 0x120;

        BusMessage Generate(CarState* carState)
        {
            CAN_120_2004_Byte1Struct field1{};
            field1.data.menu_available = 3;
            field1.data.total_number_of_blocks = 3;
            field1.data.block_number = blockNumber;

            if (carState->CurrenTime - lastMessageTime >= message.periodicityMs)
            {
                lastMessageTime = carState->CurrenTime;
                blockNumber = blockNumber < field1.data.total_number_of_blocks ? blockNumber + 1 : 1;
            }

            switch (field1.data.block_number)
            {
                case 1:
                {
                    CAN120Block1 block1{};

                    block1.data.byte2.data.e_top_up_coolant_level  = carState->AlertHistory1.data.e_top_up_coolant_level;
                    block1.data.byte2.data.s_braking_system_faulty = carState->AlertHistory1.data.s_braking_system_faulty;
                    block1.data.byte2.data.s_engine_temperature_fault_stop_the_vehicle  = carState->AlertHistory1.data.s_engine_temperature_fault_stop_the_vehicle;
                    block1.data.byte2.data.s_engine_oil_pressure_fault_stop_the_vehicle = carState->AlertHistory1.data.s_engine_oil_pressure_fault_stop_the_vehicle;
                    block1.data.byte2.data.s_power_steering_faulty = carState->AlertHistory1.data.s_power_steering_faulty;
                    block1.data.byte2.data.unknown0 = carState->AlertHistory1.data.unknown_2_0;
                    block1.data.byte2.data.unknown3 = carState->AlertHistory1.data.unknown_2_3;
                    block1.data.byte2.data.unknown5 = carState->AlertHistory1.data.unknown_2_5;

                    block1.data.byte3.data.e_top_up_engine_oil_level = carState->AlertHistory1.data.e_top_up_engine_oil_level;
                    block1.data.byte3.data.i_handbrake               = carState->AlertHistory1.data.i_handbrake;
                    block1.data.byte3.data.s_boot_open               = carState->AlertHistory1.data.s_boot_open;
                    block1.data.byte3.data.s_fuel_tank_access_not_properly_locked = carState->AlertHistory1.data.s_fuel_tank_access_not_properly_locked;
                    block1.data.byte3.data.s_rear_right_hand_door_open  = carState->AlertHistory1.data.s_rear_right_hand_door_open;
                    block1.data.byte3.data.s_rear_left_hand_door_open   = carState->AlertHistory1.data.s_rear_left_hand_door_open;
                    block1.data.byte3.data.s_front_right_hand_door_open = carState->AlertHistory1.data.s_front_right_hand_door_open;
                    block1.data.byte3.data.s_front_left_hand_door_open  = carState->AlertHistory1.data.s_front_left_hand_door_open;

                    block1.data.byte4.data.e_airbags_or_pretensioner_seat_belts_faulty        = carState->AlertHistory1.data.e_airbags_or_pretensioner_seat_belts_faulty;
                    block1.data.byte4.data.e_battery_charge_or_electrical_supply_faulty       = carState->AlertHistory1.data.e_battery_charge_or_electrical_supply_faulty;
                    block1.data.byte4.data.e_brake_pads_worn                                  = carState->AlertHistory1.data.e_brake_pads_worn;
                    block1.data.byte4.data.e_esp_asr_system_faulty                            = carState->AlertHistory1.data.e_esp_asr_system_faulty;
                    block1.data.byte4.data.e_presence_of_water_in_diesel_filter_repair_needed = carState->AlertHistory1.data.e_presence_of_water_in_diesel_filter_repair_needed;
                    block1.data.byte4.data.i_fuel_level_too_low = carState->AlertHistory1.data.i_fuel_level_too_low;
                    block1.data.byte4.data.s_rear_screen_open   = carState->AlertHistory1.data.s_rear_screen_open;
                    block1.data.byte4.data.unknown4             = carState->AlertHistory1.data.unknown_4_4;

                    block1.data.byte5.data.e_abs_braking_system_faulty                            = carState->AlertHistory1.data.e_abs_braking_system_faulty;
                    block1.data.byte5.data.e_engine_fault_repair_needed                           = carState->AlertHistory1.data.e_engine_fault_repair_needed;
                    block1.data.byte5.data.e_particle_filter_additive_level_too_low_repair_needed = carState->AlertHistory1.data.e_particle_filter_additive_level_too_low_repair_needed;
                    block1.data.byte5.data.e_risk_of_particle_filter_clogging_see_handbook        = carState->AlertHistory1.data.e_risk_of_particle_filter_clogging_see_handbook;
                    block1.data.byte5.data.e_suspension_faulty                                    = carState->AlertHistory1.data.e_suspension_faulty;
                    block1.data.byte5.data.unknown1 = carState->AlertHistory1.data.unknown_5_1;
                    block1.data.byte5.data.unknown3 = carState->AlertHistory1.data.unknown_5_3;
                    block1.data.byte5.data.unknown7 = carState->AlertHistory1.data.unknown_5_7;

                    block1.data.byte6.data.e_cruise_control_system_faulty = carState->AlertHistory1.data.e_cruise_control_system_faulty;
                    block1.data.byte6.data.e_electronic_anti_theft_faulty = carState->AlertHistory1.data.e_electronic_anti_theft_faulty;
                    block1.data.byte6.data.unknown0 = carState->AlertHistory1.data.roof_manoeuvre_impossible_system_temperature_too_high;
                    block1.data.byte6.data.unknown1 = carState->AlertHistory1.data.unknown_6_1;
                    block1.data.byte6.data.unknown3 = carState->AlertHistory1.data.steering_lock_fault_have_the_vehicle_repaired;
                    block1.data.byte6.data.unknown4 = carState->AlertHistory1.data.retractable_roof_mechanism_fault;
                    block1.data.byte6.data.unknown5 = carState->AlertHistory1.data.check_the_central_stop_lamp;
                    block1.data.byte6.data.unknown7 = carState->AlertHistory1.data.pre_heating_pre_ventilation_deactivated_battery_charge_low;

                    block1.data.byte7.data.i_automatic_headlamp_adjustment_faulty = carState->AlertHistory1.data.i_automatic_headlamp_adjustment_faulty;
                    block1.data.byte7.data.i_remote_control_battery_flat          = carState->AlertHistory1.data.i_remote_control_battery_flat;
                    block1.data.byte7.data.i_screen_wash_fluid_level_too_low      = carState->AlertHistory1.data.i_screen_wash_fluid_level_too_low;
                    block1.data.byte7.data.unknown2 = carState->AlertHistory1.data.hybrid_system_fault_speed_restricted_repair_needed;
                    block1.data.byte7.data.unknown3 = carState->AlertHistory1.data.hybrid_system_fault_repair_needed;
                    block1.data.byte7.data.unknown5 = carState->AlertHistory1.data.hybrid_system_fault_stop_the_vehicle_see_handbook;
                    block1.data.byte7.data.unknown6 = carState->AlertHistory1.data.roof_manoeuvre_impossible_apply_handbrake;
                    block1.data.byte7.data.unknown7 = carState->AlertHistory1.data.roof_manoeuvre_impossible_start_the_engine;

                    block1.data.byte8.data.e_ambient_brightness_sensor_faulty = carState->AlertHistory1.data.e_ambient_brightness_sensor_faulty;
                    block1.data.byte8.data.e_tyre_pressures_not_monitored     = carState->AlertHistory1.data.e_tyre_pressures_not_monitored;
                    block1.data.byte8.data.unknown0 = carState->AlertHistory1.data.unknown_8_0;
                    block1.data.byte8.data.unknown1 = carState->AlertHistory1.data.unknown_8_1;
                    block1.data.byte8.data.unknown2 = carState->AlertHistory1.data.unknown_8_2;
                    block1.data.byte8.data.unknown4 = carState->AlertHistory1.data.unknown_8_4;
                    block1.data.byte8.data.unknown6 = carState->AlertHistory1.data.pre_heating_pre_ventilation_deactivated_set_the_clock;
                    block1.data.byte8.data.unknown7 = carState->AlertHistory1.data.unknown_8_7;

                    message.data[0] = field1.asByte;
                    message.data[1] = block1.raw[1];
                    message.data[2] = block1.raw[2];
                    message.data[3] = block1.raw[3];
                    message.data[4] = block1.raw[4];
                    message.data[5] = block1.raw[5];
                    message.data[6] = block1.raw[6];
                    message.data[7] = block1.raw[7];
                    break;
                }

                case 2:
                {
                    CAN120Block2 block2{};

                    block2.data.byte2.data.e_sidelamp_bulbs_faulty   = carState->AlertHistory2.data.e_sidelamp_bulbs_faulty;
                    block2.data.byte2.data.s_front_lh_tyre_punctured = carState->AlertHistory2.data.s_front_lh_tyre_punctured;
                    block2.data.byte2.data.s_front_rh_tyre_punctured = carState->AlertHistory2.data.s_front_rh_tyre_punctured;
                    block2.data.byte2.data.s_rear_lh_tyre_punctured  = carState->AlertHistory2.data.s_rear_lh_tyre_punctured;
                    block2.data.byte2.data.s_rear_rh_tyre_punctured  = carState->AlertHistory2.data.s_rear_rh_tyre_punctured;
                    block2.data.byte2.data.unknown5 = carState->AlertHistory2.data.unknown_2_5;
                    block2.data.byte2.data.unknown6 = carState->AlertHistory2.data.unknown_2_6;
                    block2.data.byte2.data.unknown7 = carState->AlertHistory2.data.unknown_2_7;

                    block2.data.byte3.data.e_dipped_beam_bulbs_faulty1 = carState->AlertHistory2.data.e_dipped_beam_bulbs_faulty1;
                    block2.data.byte3.data.e_dipped_beam_bulbs_faulty2 = carState->AlertHistory2.data.e_dipped_beam_bulbs_faulty2;
                    block2.data.byte3.data.e_main_beam_bulbs_faulty1   = carState->AlertHistory2.data.e_main_beam_bulbs_faulty1;
                    block2.data.byte3.data.e_main_beam_bulbs_faulty2   = carState->AlertHistory2.data.e_main_beam_bulbs_faulty2;
                    block2.data.byte3.data.e_sidelamp_bulbs_faulty1    = carState->AlertHistory2.data.e_sidelamp_bulbs_faulty1;
                    block2.data.byte3.data.e_sidelamp_bulbs_faulty2    = carState->AlertHistory2.data.e_sidelamp_bulbs_faulty2;
                    block2.data.byte3.data.e_sidelamp_bulbs_faulty3    = carState->AlertHistory2.data.e_sidelamp_bulbs_faulty3;
                    block2.data.byte3.data.e_rear_right_hand_brake_light_bulb_faulty = carState->AlertHistory2.data.e_rear_right_hand_brake_light_bulb_faulty;

                    block2.data.byte4.data.e_direction_indicators_faulty1 = carState->AlertHistory2.data.e_direction_indicators_faulty1;
                    block2.data.byte4.data.e_direction_indicators_faulty2 = carState->AlertHistory2.data.e_direction_indicators_faulty2;
                    block2.data.byte4.data.e_direction_indicators_faulty3 = carState->AlertHistory2.data.e_direction_indicators_faulty3;
                    block2.data.byte4.data.e_foglamp_bulbs_faulty1        = carState->AlertHistory2.data.e_foglamp_bulbs_faulty1;
                    block2.data.byte4.data.e_foglamp_bulbs_faulty2        = carState->AlertHistory2.data.e_foglamp_bulbs_faulty2;
                    block2.data.byte4.data.e_foglamp_bulbs_faulty3        = carState->AlertHistory2.data.e_foglamp_bulbs_faulty3;
                    block2.data.byte4.data.e_foglamp_bulbs_faulty4        = carState->AlertHistory2.data.e_foglamp_bulbs_faulty4;
                    block2.data.byte4.data.e_rear_left_hand_brake_light_bulb_faulty = carState->AlertHistory2.data.e_rear_left_hand_brake_light_bulb_faulty;

                    block2.data.byte5.data.e_direction_indicators_faulty           = carState->AlertHistory2.data.e_direction_indicators_faulty;
                    block2.data.byte5.data.e_left_hand_reversing_lamp_bulb_faulty  = carState->AlertHistory2.data.e_left_hand_reversing_lamp_bulb_faulty;
                    block2.data.byte5.data.e_right_hand_reversing_lamp_bulb_faulty = carState->AlertHistory2.data.e_right_hand_reversing_lamp_bulb_faulty;
                    block2.data.byte5.data.unknown0 = carState->AlertHistory2.data.unknown_5_0;
                    block2.data.byte5.data.unknown1 = carState->AlertHistory2.data.unknown_5_1;
                    block2.data.byte5.data.unknown2 = carState->AlertHistory2.data.unknown_5_2;
                    block2.data.byte5.data.unknown3 = carState->AlertHistory2.data.unknown_5_3;
                    block2.data.byte5.data.unknown4 = carState->AlertHistory2.data.unknown_5_4;

                    block2.data.byte6.data.e_parking_assistance_system_faulty = carState->AlertHistory2.data.e_parking_assistance_system_faulty;
                    block2.data.byte6.data.e_tyre_pressures_too_low1          = carState->AlertHistory2.data.e_tyre_pressures_too_low1;
                    block2.data.byte6.data.e_tyre_pressures_too_low2          = carState->AlertHistory2.data.e_tyre_pressures_too_low2;
                    block2.data.byte6.data.unknown2 = carState->AlertHistory2.data.unknown_6_2;
                    block2.data.byte6.data.unknown3 = carState->AlertHistory2.data.unknown_6_3;
                    block2.data.byte6.data.unknown5 = carState->AlertHistory2.data.unknown_6_5;
                    block2.data.byte6.data.unknown6 = carState->AlertHistory2.data.unknown_6_6;
                    block2.data.byte6.data.unknown7 = carState->AlertHistory2.data.unknown_6_7;

                    block2.data.byte6.data.e_parking_assistance_system_faulty = carState->AlertHistory2.data.e_parking_assistance_system_faulty;
                    block2.data.byte7.data.e_tyre_pressures_too_low3          = carState->AlertHistory2.data.e_tyre_pressures_too_low3;
                    block2.data.byte7.data.unknown0 = carState->AlertHistory2.data.unknown_7_0;
                    block2.data.byte7.data.unknown1 = carState->AlertHistory2.data.emissions_fault;
                    block2.data.byte7.data.unknown2 = carState->AlertHistory2.data.emissions_fault_starting_prevented;
                    block2.data.byte7.data.unknown3 = carState->AlertHistory2.data.unknown_7_3;
                    block2.data.byte7.data.unknown4 = carState->AlertHistory2.data.unknown_7_4;
                    block2.data.byte7.data.unknown5 = carState->AlertHistory2.data.switch_off_the_lamps;

                    block2.data.byte8.data.e_front_left_hand_door_open  = carState->AlertHistory2.data.e_front_left_hand_door_open;
                    block2.data.byte8.data.e_front_right_hand_door_open = carState->AlertHistory2.data.e_front_right_hand_door_open;
                    block2.data.byte8.data.e_rear_left_hand_door_open   = carState->AlertHistory2.data.e_rear_left_hand_door_open;
                    block2.data.byte8.data.e_rear_right_hand_door_open  = carState->AlertHistory2.data.e_rear_right_hand_door_open;
                    block2.data.byte8.data.i_place_automatic_gearbox_in_position_p = carState->AlertHistory2.data.i_place_automatic_gearbox_in_position_p;
                    block2.data.byte8.data.i_risk_of_ice = carState->AlertHistory2.data.i_risk_of_ice;
                    block2.data.byte8.data.unknown6      = carState->AlertHistory2.data.unknown_8_6;
                    block2.data.byte8.data.unknown7      = carState->AlertHistory2.data.unknown_8_7;

                    message.data[0] = field1.asByte;
                    message.data[1] = block2.raw[1];
                    message.data[2] = block2.raw[2];
                    message.data[3] = block2.raw[3];
                    message.data[4] = block2.raw[4];
                    message.data[5] = block2.raw[5];
                    message.data[6] = block2.raw[6];
                    message.data[7] = block2.raw[7];
                    break;
                }
                case 3:
                {
                    CAN120Block3 block3{};

                    block3.data.byte2.data.e_bonnet_open = carState->AlertHistory3.data.e_bonnet_open;
                    block3.data.byte2.data.e_boot_open   = carState->AlertHistory3.data.e_boot_open;
                    block3.data.byte2.data.e_fuel_tank_access_not_properly_locked    = carState->AlertHistory3.data.e_fuel_tank_access_not_properly_locked;
                    block3.data.byte2.data.e_high_speed_check_tyre_pressures_correct = carState->AlertHistory3.data.e_high_speed_check_tyre_pressures_correct;
                    block3.data.byte2.data.e_rear_screen_open = carState->AlertHistory3.data.e_rear_screen_open;
                    block3.data.byte2.data.unknown0 = carState->AlertHistory3.data.unknown_2_0;
                    block3.data.byte2.data.unknown2 = carState->AlertHistory3.data.unknown_2_2;
                    block3.data.byte2.data.unknown3 = carState->AlertHistory3.data.unknown_2_3;

                    block3.data.byte3.data.e_adjustable_wing_fault_speed_restricted_see_handbook = carState->AlertHistory3.data.e_adjustable_wing_fault_speed_restricted_see_handbook;
                    block3.data.byte3.data.e_directional_headlamps_faulty = carState->AlertHistory3.data.e_directional_headlamps_faulty;
                    block3.data.byte3.data.e_parking_brake_faulty         = carState->AlertHistory3.data.e_parking_brake_faulty;
                    block3.data.byte3.data.s_bonnet_open                  = carState->AlertHistory3.data.s_bonnet_open;
                    block3.data.byte3.data.unknown0 = carState->AlertHistory3.data.unknown_3_0;
                    block3.data.byte3.data.unknown5 = carState->AlertHistory3.data.unknown_3_5;
                    block3.data.byte3.data.unknown6 = carState->AlertHistory3.data.unknown_3_6;
                    block3.data.byte3.data.unknown7 = carState->AlertHistory3.data.unknown_3_7;

                    block3.data.byte4.data.e_gearbox_fault_repair_needed = carState->AlertHistory3.data.e_gearbox_fault_repair_needed;
                    block3.data.byte4.data.unknown0 = carState->AlertHistory3.data.unknown_4_0;
                    block3.data.byte4.data.unknown1 = carState->AlertHistory3.data.unknown_4_1;
                    block3.data.byte4.data.unknown3 = carState->AlertHistory3.data.unknown_4_3;
                    block3.data.byte4.data.unknown4 = carState->AlertHistory3.data.unknown_4_4;
                    block3.data.byte4.data.unknown5 = carState->AlertHistory3.data.unknown_4_5;
                    block3.data.byte4.data.unknown6 = carState->AlertHistory3.data.unknown_4_6;
                    block3.data.byte4.data.unknown7 = carState->AlertHistory3.data.unknown_4_7;

                    block3.data.byte5.data.e_anti_wander_system_lane_crossing_warning_device_faulty = carState->AlertHistory3.data.e_anti_wander_system_lane_crossing_warning_device_faulty;
                    block3.data.byte5.data.e_engine_fault_repair_needed = carState->AlertHistory3.data.e_engine_fault_repair_needed;
                    block3.data.byte5.data.e_tyre_pressures_too_low     = carState->AlertHistory3.data.e_tyre_pressures_too_low;
                    block3.data.byte5.data.unknown0 = carState->AlertHistory3.data.unknown_5_0;
                    block3.data.byte5.data.unknown1 = carState->AlertHistory3.data.unknown_5_1;
                    block3.data.byte5.data.unknown3 = carState->AlertHistory3.data.unknown_5_3;
                    block3.data.byte5.data.unknown5 = carState->AlertHistory3.data.unknown_5_5;
                    block3.data.byte5.data.unknown7 = carState->AlertHistory3.data.unknown_5_7;

                    block3.data.byte6.data.i_front_lh_tyre_not_monitored = carState->AlertHistory3.data.i_front_lh_tyre_not_monitored;
                    block3.data.byte6.data.i_front_rh_tyre_not_monitored = carState->AlertHistory3.data.i_front_rh_tyre_not_monitored;
                    block3.data.byte6.data.i_rear_rh_tyre_not_monitored  = carState->AlertHistory3.data.i_rear_rh_tyre_not_monitored;
                    block3.data.byte6.data.i_rear_lh_tyre_not_monitored  = carState->AlertHistory3.data.i_rear_lh_tyre_not_monitored;
                    block3.data.byte6.data.unknown4 = carState->AlertHistory3.data.unknown_6_4;
                    block3.data.byte6.data.unknown5 = carState->AlertHistory3.data.unknown_6_5;
                    block3.data.byte6.data.unknown6 = carState->AlertHistory3.data.unknown_6_6;
                    block3.data.byte6.data.unknown7 = carState->AlertHistory3.data.unknown_6_7;

                    block3.data.byte7.data.e_engine_fault_stop_the_vehicle = carState->AlertHistory3.data.e_engine_fault_stop_the_vehicle;
                    block3.data.byte7.data.e_power_steering_faulty         = carState->AlertHistory3.data.e_power_steering_faulty;
                    block3.data.byte7.data.e_power_steering_faulty         = carState->AlertHistory3.data.e_power_steering_faulty;
                    block3.data.byte7.data.unknown0 = carState->AlertHistory3.data.tyre_under_inflation_detection_fault;
                    block3.data.byte7.data.unknown1 = carState->AlertHistory3.data.unknown_7_1;
                    block3.data.byte7.data.unknown3 = carState->AlertHistory3.data.e_engine_fault_stop_the_vehicle;
                    block3.data.byte7.data.unknown4 = carState->AlertHistory3.data.unknown_7_4;
                    block3.data.byte7.data.unknown5 = carState->AlertHistory3.data.unknown_7_5;

                    block3.data.byte8.data.unknown0 = carState->AlertHistory3.data.refill_emissions_additive_starting_prevented;
                    block3.data.byte8.data.unknown1 = carState->AlertHistory3.data.refill_emissions_additive1;
                    block3.data.byte8.data.unknown2 = carState->AlertHistory3.data.refill_emissions_additive2;
                    block3.data.byte8.data.unknown3 = carState->AlertHistory3.data.unknown_8_3;
                    block3.data.byte8.data.unknown4 = carState->AlertHistory3.data.left_hand_front_tyre_under_inflated_adjust_tyre_pressures_then_reinitialise;
                    block3.data.byte8.data.unknown5 = carState->AlertHistory3.data.right_hand_front_tyre_under_inflated_adjust_tyre_pressures_then_reinitialise;
                    block3.data.byte8.data.unknown6 = carState->AlertHistory3.data.left_hand_rear_tyre_under_inflated_adjust_tyre_pressures_then_reinitialise;
                    block3.data.byte8.data.unknown7 = carState->AlertHistory3.data.right_hand_rear_tyre_under_inflated_adjust_tyre_pressures_then_reinitialise;

                    message.data[0] = field1.asByte;
                    message.data[1] = block3.raw[1];
                    message.data[2] = block3.raw[2];
                    message.data[3] = block3.raw[3];
                    message.data[4] = block3.raw[4];
                    message.data[5] = block3.raw[5];
                    message.data[6] = block3.raw[6];
                    message.data[7] = block3.raw[7];
                    break;
                }

                default:
                    break;
            }

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {

        }
};
#endif
