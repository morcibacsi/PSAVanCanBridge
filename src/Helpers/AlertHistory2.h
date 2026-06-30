#pragma once

#include <cstdint>

union AlertHistory2Struct {
    struct {
        uint8_t                        : 2; // bit 0-1
        uint8_t menu_available         : 2; // bit 2-3
        uint8_t total_number_of_blocks : 2; // bit 4-5
        uint8_t block_number           : 2; // bit 6-7

        uint8_t e_sidelamp_bulbs_faulty   : 1; // bit 0
        uint8_t s_rear_lh_tyre_punctured  : 1; // bit 1
        uint8_t s_rear_rh_tyre_punctured  : 1; // bit 2
        uint8_t s_front_rh_tyre_punctured : 1; // bit 3
        uint8_t s_front_lh_tyre_punctured : 1; // bit 4
        uint8_t unknown_2_5               : 1; // bit 5
        uint8_t unknown_2_6               : 1; // bit 6
        uint8_t unknown_2_7               : 1; // bit 7

        uint8_t e_rear_right_hand_brake_light_bulb_faulty : 1;
        uint8_t e_main_beam_bulbs_faulty1      : 1;
        uint8_t e_main_beam_bulbs_faulty2      : 1;
        uint8_t e_dipped_beam_bulbs_faulty1    : 1;
        uint8_t e_dipped_beam_bulbs_faulty2    : 1;
        uint8_t e_sidelamp_bulbs_faulty1       : 1;
        uint8_t e_sidelamp_bulbs_faulty2       : 1;
        uint8_t e_sidelamp_bulbs_faulty3       : 1;

        uint8_t e_direction_indicators_faulty1 : 1;
        uint8_t e_direction_indicators_faulty2 : 1;
        uint8_t e_direction_indicators_faulty3 : 1;
        uint8_t e_foglamp_bulbs_faulty1        : 1;//front
        uint8_t e_foglamp_bulbs_faulty2        : 1;//front
        uint8_t e_foglamp_bulbs_faulty3        : 1;//rear
        uint8_t e_foglamp_bulbs_faulty4        : 1;//rear
        uint8_t e_rear_left_hand_brake_light_bulb_faulty : 1;

        uint8_t unknown_5_0                             : 1; // bit 0
        uint8_t unknown_5_1                             : 1; // bit 1
        uint8_t unknown_5_2                             : 1; // bit 2
        uint8_t unknown_5_3                             : 1; // bit 3
        uint8_t unknown_5_4                             : 1; // bit 4
        uint8_t e_left_hand_reversing_lamp_bulb_faulty  : 1; // bit 5
        uint8_t e_right_hand_reversing_lamp_bulb_faulty : 1; // bit 6
        uint8_t e_direction_indicators_faulty           : 1; // bit 7

        uint8_t e_tyre_pressures_too_low1             : 1; // bit 0
        uint8_t e_tyre_pressures_too_low2             : 1; // bit 1
        uint8_t unknown_6_2                           : 1; // bit 2
        uint8_t unknown_6_3                           : 1; // bit 3
        uint8_t e_parking_assistance_system_faulty    : 1; // bit 4
        uint8_t unknown_6_5                           : 1; // bit 5
        uint8_t unknown_6_6                           : 1; // bit 6
        uint8_t unknown_6_7                           : 1; // bit 7

        uint8_t unknown_7_0               : 1; // bit 0
        uint8_t emissions_fault           : 1; // bit 1 //AEE2010
        uint8_t emissions_fault_starting_prevented : 1; // bit 2 //AEE2010
        uint8_t unknown_7_3               : 1; // bit 3
        uint8_t unknown_7_4               : 1; // bit 4
        uint8_t switch_off_the_lamps      : 1; // bit 5 //AEE2010
        uint8_t e_tyre_pressures_too_low3 : 1; // bit 6
        uint8_t e_tyre_pressures_too_low4 : 1; // bit 7

        uint8_t e_rear_left_hand_door_open              : 1; // bit 0
        uint8_t e_rear_right_hand_door_open             : 1; // bit 1
        uint8_t e_front_left_hand_door_open             : 1; // bit 2
        uint8_t e_front_right_hand_door_open            : 1; // bit 3
        uint8_t i_risk_of_ice                           : 1; // bit 4
        uint8_t i_place_automatic_gearbox_in_position_p : 1; // bit 5
        uint8_t unknown_8_6                             : 1; // bit 6
        uint8_t unknown_8_7                             : 1; // bit 7
    } data;
    uint64_t asUint64;
};
