#pragma once

#include <cstdint>

union AlertHistory1Struct {
    struct {
        uint8_t                        : 2; // bit 0-1
        uint8_t menu_available         : 2; // bit 2-3
        uint8_t total_number_of_blocks : 2; // bit 4-5
        uint8_t block_number           : 2; // bit 6-7

        uint8_t unknown_2_0                                  : 1; // bit 0
        uint8_t e_top_up_coolant_level                       : 1; // bit 1
        uint8_t s_power_steering_faulty                      : 1; // bit 2
        uint8_t unknown_2_3                                  : 1; // bit 3
        uint8_t s_braking_system_faulty                      : 1; // bit 4
        uint8_t unknown_2_5                                  : 1; // bit 5
        uint8_t s_engine_temperature_fault_stop_the_vehicle  : 1; // bit 6
        uint8_t s_engine_oil_pressure_fault_stop_the_vehicle : 1; // bit 7

        uint8_t s_boot_open                            : 1; // bit 0
        uint8_t s_fuel_tank_access_not_properly_locked : 1; // bit 1
        uint8_t s_rear_right_hand_door_open            : 1; // bit 2
        uint8_t s_rear_left_hand_door_open             : 1; // bit 3
        uint8_t s_front_right_hand_door_open           : 1; // bit 4
        uint8_t s_front_left_hand_door_open            : 1; // bit 5
        uint8_t i_handbrake                            : 1; // bit 6
        uint8_t e_top_up_engine_oil_level              : 1; // bit 7

        uint8_t e_airbags_or_pretensioner_seat_belts_faulty        : 1; // bit 0
        uint8_t i_fuel_level_too_low                               : 1; // bit 1
        uint8_t e_brake_pads_worn                                  : 1; // bit 2
        uint8_t e_presence_of_water_in_diesel_filter_repair_needed : 1; // bit 3
        uint8_t unknown_4_4                                        : 1; // bit 4
        uint8_t e_battery_charge_or_electrical_supply_faulty       : 1; // bit 5
        uint8_t e_esp_asr_system_faulty                            : 1; // bit 6
        uint8_t s_rear_screen_open                                 : 1; // bit 7

        uint8_t e_suspension_faulty                                    : 1; // bit 0
        uint8_t unknown_5_1                                            : 1; // bit 1
        uint8_t e_particle_filter_additive_level_too_low_repair_needed : 1; // bit 2
        uint8_t unknown_5_3                                            : 1; // bit 3
        uint8_t e_risk_of_particle_filter_clogging_see_handbook        : 1; // bit 4
        uint8_t e_abs_braking_system_faulty                            : 1; // bit 5
        uint8_t e_engine_fault_repair_needed                           : 1; // bit 6
        uint8_t unknown_5_7                                            : 1; // bit 7

        uint8_t roof_manoeuvre_impossible_system_temperature_too_high      : 1; // bit 0 //AEE2010
        uint8_t unknown_6_1                                                : 1; // bit 1
        uint8_t e_electronic_anti_theft_faulty                             : 1; // bit 2
        uint8_t steering_lock_fault_have_the_vehicle_repaired              : 1; // bit 3 //AEE2010
        uint8_t retractable_roof_mechanism_fault                           : 1; // bit 4 //AEE2010
        uint8_t check_the_central_stop_lamp                                : 1; // bit 5 //AEE2010
        uint8_t e_cruise_control_system_faulty                             : 1; // bit 6
        uint8_t pre_heating_pre_ventilation_deactivated_battery_charge_low : 1; // bit 7 //AEE2010

        uint8_t i_remote_control_battery_flat                      : 1; // bit 0
        uint8_t i_screen_wash_fluid_level_too_low                  : 1; // bit 1
        uint8_t hybrid_system_fault_speed_restricted_repair_needed : 1; // bit 2 //AEE2010
        uint8_t hybrid_system_fault_repair_needed                  : 1; // bit 3 //AEE2010
        uint8_t i_automatic_headlamp_adjustment_faulty             : 1; // bit 4
        uint8_t hybrid_system_fault_stop_the_vehicle_see_handbook  : 1; // bit 5 //AEE2010
        uint8_t roof_manoeuvre_impossible_apply_handbrake          : 1; // bit 6 //AEE2010
        uint8_t roof_manoeuvre_impossible_start_the_engine         : 1; // bit 7 //AEE2010

        uint8_t unknown_8_0                        : 1; // bit 0
        uint8_t unknown_8_1                        : 1; // bit 1
        uint8_t unknown_8_2                        : 1; // bit 2
        uint8_t e_tyre_pressures_not_monitored     : 1; // bit 3
        uint8_t unknown_8_4                        : 1; // bit 4
        uint8_t e_ambient_brightness_sensor_faulty : 1; // bit 5 trailer_connection_fault_repair_needed //AEE2010
        uint8_t pre_heating_pre_ventilation_deactivated_set_the_clock : 1; // bit 6 //AEE2010
        uint8_t unknown_8_7                        : 1; // bit 7
    } data;
    uint64_t asUint64;
};
