#pragma once

#include <cstdint>

union AlertHistory3Struct {
    struct {
        uint8_t                        : 2; // bit 0-1
        uint8_t menu_available         : 2; // bit 2-3
        uint8_t total_number_of_blocks : 2; // bit 4-5
        uint8_t block_number           : 2; // bit 6-7

        uint8_t unknown_2_0                               : 1; // bit 0
        uint8_t e_high_speed_check_tyre_pressures_correct : 1; // bit 1
        uint8_t unknown_2_2                               : 1; // bit 2
        uint8_t unknown_2_3                               : 1; // bit 3
        uint8_t e_fuel_tank_access_not_properly_locked    : 1; // bit 4
        uint8_t e_rear_screen_open                        : 1; // bit 5
        uint8_t e_bonnet_open                             : 1; // bit 6
        uint8_t e_boot_open                               : 1; // bit 7

        uint8_t unknown_3_0                                           : 1; // bit 0
        uint8_t e_directional_headlamps_faulty                        : 1; // bit 1
        uint8_t s_bonnet_open                                         : 1; // bit 2
        uint8_t e_adjustable_wing_fault_speed_restricted_see_handbook : 1; // bit 3
        uint8_t e_parking_brake_faulty                                : 1; // bit 4
        uint8_t unknown_3_5                                           : 1; // bit 5
        uint8_t unknown_3_6                                           : 1; // bit 6
        uint8_t unknown_3_7                                           : 1; // bit 7

        uint8_t unknown_4_0                   : 1; // bit 0
        uint8_t unknown_4_1                   : 1; // bit 1
        uint8_t e_gearbox_fault_repair_needed : 1; // bit 2
        uint8_t unknown_4_3                   : 1; // bit 3
        uint8_t unknown_4_4                   : 1; // bit 4
        uint8_t unknown_4_5                   : 1; // bit 5
        uint8_t unknown_4_6                   : 1; // bit 6
        uint8_t unknown_4_7                   : 1; // bit 7

        uint8_t unknown_5_0                   : 1; // bit 0
        uint8_t unknown_5_1                   : 1; // bit 1
        uint8_t e_engine_fault_repair_needed  : 1; // bit 2
        uint8_t unknown_5_3                   : 1; // bit 3
        uint8_t e_tyre_pressures_too_low      : 1; // bit 4
        uint8_t unknown_5_5                   : 1; // bit 5
        uint8_t e_anti_wander_system_lane_crossing_warning_device_faulty : 1; // bit 6
        uint8_t unknown_5_7                   : 1; // bit 7

        uint8_t i_rear_lh_tyre_not_monitored  : 1; // bit 0
        uint8_t i_rear_rh_tyre_not_monitored  : 1; // bit 1
        uint8_t i_front_rh_tyre_not_monitored : 1; // bit 2
        uint8_t i_front_lh_tyre_not_monitored : 1; // bit 3
        uint8_t unknown_6_4                   : 1; // bit 4
        uint8_t unknown_6_5                   : 1; // bit 5
        uint8_t unknown_6_6                   : 1; // bit 6
        uint8_t unknown_6_7                   : 1; // bit 7

        uint8_t tyre_under_inflation_detection_fault : 1; // bit 0
        uint8_t unknown_7_1                     : 1; // bit 1
        uint8_t e_engine_fault_stop_the_vehicle : 1; // bit 2
        uint8_t inter_vehicles_time_measuring_fault : 1; // bit 3 //AEE2010
        uint8_t unknown_7_4                     : 1; // bit 4
        uint8_t unknown_7_5                     : 1; // bit 5
        uint8_t e_power_steering_faulty         : 1; // bit 6
        uint8_t e_suspension_faulty             : 1; // bit 7

        uint8_t refill_emissions_additive_starting_prevented : 1; // bit 0 //AEE2010
        uint8_t refill_emissions_additive1                   : 1; // bit 1 //AEE2010
        uint8_t refill_emissions_additive2                   : 1; // bit 2
        uint8_t unknown_8_3                                  : 1; // bit 3
        uint8_t left_hand_front_tyre_under_inflated_adjust_tyre_pressures_then_reinitialise  : 1; // bit 4 //AEE2010
        uint8_t right_hand_front_tyre_under_inflated_adjust_tyre_pressures_then_reinitialise : 1; // bit 5 //AEE2010
        uint8_t left_hand_rear_tyre_under_inflated_adjust_tyre_pressures_then_reinitialise   : 1; // bit 6 //AEE2010
        uint8_t right_hand_rear_tyre_under_inflated_adjust_tyre_pressures_then_reinitialise  : 1; // bit 7 //AEE2010
    } data;
    uint64_t asUint64;
};
