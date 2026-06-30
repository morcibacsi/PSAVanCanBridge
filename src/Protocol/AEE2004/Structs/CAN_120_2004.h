#pragma once

#ifndef _2004_120_h
    #define _2004_120_h

#include <stdint.h>

// CANID: 120
const uint16_t CAN2004_ID_120 = 0x120;
const uint8_t CAN2004_ID_120_LEN = 8;

const uint8_t MENU_NOT_AVAILABLE_AND_ACTIVATION_IMPOSSIBLE = 0;
const uint8_t MENU_NOT_AVAILABLE_ACTIVATION_POSSIBLE       = 1;
const uint8_t MENU_AVAILABLE_AND_ACTIVATION_IMPOSSIBLE     = 2;
const uint8_t MENU_AVAILABLE_AND_ACTIVATION_POSSIBLE       = 3;

const uint8_t TOTAL_BLOCK_NUMBER_INVALID = 0;
const uint8_t TOTAL_BLOCK_NUMBER_1       = 1;
const uint8_t TOTAL_BLOCK_NUMBER_2       = 2;
const uint8_t TOTAL_BLOCK_NUMBER_3       = 3;

const uint8_t BLOCK_NUMBER_INVALID = 0;
const uint8_t BLOCK_NUMBER_1       = 1;
const uint8_t BLOCK_NUMBER_2       = 2;
const uint8_t BLOCK_NUMBER_3       = 3;

union CAN_120_2004_Byte1Struct {
    struct {
        uint8_t                        : 2; // bit 0-1
        uint8_t menu_available         : 2; // bit 2-3
        uint8_t total_number_of_blocks : 2; // bit 4-5
        uint8_t block_number           : 2; // bit 6-7
    } data;
    uint8_t asByte;
};

// Block 1 ---------------------------------------------

union CAN_120_2004_Block1Byte2Struct {
    struct
    {
        uint8_t unknown0                                     : 1; // bit 0
        uint8_t e_top_up_coolant_level                       : 1; // bit 1
        uint8_t s_power_steering_faulty                      : 1; // bit 2
        uint8_t unknown3                                     : 1; // bit 3
        uint8_t s_braking_system_faulty                      : 1; // bit 4
        uint8_t unknown5                                     : 1; // bit 5
        uint8_t s_engine_temperature_fault_stop_the_vehicle  : 1; // bit 6
        uint8_t s_engine_oil_pressure_fault_stop_the_vehicle : 1; // bit 7
    } data;
    uint8_t asByte;
};

union CAN_120_2004_Block1Byte3Struct {
    struct
    {
        uint8_t s_boot_open                            : 1; // bit 0
        uint8_t s_fuel_tank_access_not_properly_locked : 1; // bit 1
        uint8_t s_rear_right_hand_door_open            : 1; // bit 2
        uint8_t s_rear_left_hand_door_open             : 1; // bit 3
        uint8_t s_front_right_hand_door_open           : 1; // bit 4
        uint8_t s_front_left_hand_door_open            : 1; // bit 5
        uint8_t i_handbrake                            : 1; // bit 6
        uint8_t e_top_up_engine_oil_level              : 1; // bit 7
    } data;
    uint8_t asByte;
};

union CAN_120_2004_Block1Byte4Struct {
    struct
    {
        uint8_t e_airbags_or_pretensioner_seat_belts_faulty        : 1; // bit 0
        uint8_t i_fuel_level_too_low                               : 1; // bit 1
        uint8_t e_brake_pads_worn                                  : 1; // bit 2
        uint8_t e_presence_of_water_in_diesel_filter_repair_needed : 1; // bit 3
        uint8_t unknown4                                           : 1; // bit 4
        uint8_t e_battery_charge_or_electrical_supply_faulty       : 1; // bit 5
        uint8_t e_esp_asr_system_faulty                            : 1; // bit 6
        uint8_t s_rear_screen_open                                 : 1; // bit 7
    } data;
    uint8_t asByte;
};

union CAN_120_2004_Block1Byte5Struct {
    struct
    {
        uint8_t e_suspension_faulty                                    : 1; // bit 0
        uint8_t unknown1                                               : 1; // bit 1
        uint8_t e_particle_filter_additive_level_too_low_repair_needed : 1; // bit 2
        uint8_t unknown3                                               : 1; // bit 3
        uint8_t e_risk_of_particle_filter_clogging_see_handbook        : 1; // bit 4
        uint8_t e_abs_braking_system_faulty                            : 1; // bit 5
        uint8_t e_engine_fault_repair_needed                           : 1; // bit 6
        uint8_t unknown7                                               : 1; // bit 7
    } data;
    uint8_t asByte;
};

union CAN_120_2004_Block1Byte6Struct {
    struct
    {
        uint8_t unknown0                       : 1; // bit 0
        uint8_t unknown1                       : 1; // bit 1
        uint8_t e_electronic_anti_theft_faulty : 1; // bit 2
        uint8_t unknown3                       : 1; // bit 3
        uint8_t unknown4                       : 1; // bit 4
        uint8_t unknown5                       : 1; // bit 5
        uint8_t e_cruise_control_system_faulty : 1; // bit 6
        uint8_t unknown7                       : 1; // bit 7
    } data;
    uint8_t asByte;
};

union CAN_120_2004_Block1Byte7Struct {
    struct
    {
        uint8_t i_remote_control_battery_flat          : 1; // bit 0
        uint8_t i_screen_wash_fluid_level_too_low      : 1; // bit 1
        uint8_t unknown2                               : 1; // bit 2
        uint8_t unknown3                               : 1; // bit 3
        uint8_t i_automatic_headlamp_adjustment_faulty : 1; // bit 4
        uint8_t unknown5                               : 1; // bit 5
        uint8_t unknown6                               : 1; // bit 6
        uint8_t unknown7                               : 1; // bit 7
    } data;
    uint8_t asByte;
};

union CAN_120_2004_Block1Byte8Struct {
    struct
    {
        uint8_t unknown0                           : 1; // bit 0
        uint8_t unknown1                           : 1; // bit 1
        uint8_t unknown2                           : 1; // bit 2
        uint8_t e_tyre_pressures_not_monitored     : 1; // bit 3
        uint8_t unknown4                           : 1; // bit 4
        uint8_t e_ambient_brightness_sensor_faulty : 1; // bit 5
        uint8_t unknown6                           : 1; // bit 6
        uint8_t unknown7                           : 1; // bit 7
    } data;
    uint8_t asByte;
};

union CAN120Block1 {
    struct {
        uint8_t placeholder;
        CAN_120_2004_Block1Byte2Struct byte2;
        CAN_120_2004_Block1Byte3Struct byte3;
        CAN_120_2004_Block1Byte4Struct byte4;
        CAN_120_2004_Block1Byte5Struct byte5;
        CAN_120_2004_Block1Byte6Struct byte6;
        CAN_120_2004_Block1Byte7Struct byte7;
        CAN_120_2004_Block1Byte8Struct byte8;
    } data;

    uint64_t asUint64;
    uint8_t raw[8];
};

// Block 2 ---------------------------------------------

union CAN_120_2004_Block2Byte2Struct {
    struct
    {
        uint8_t e_sidelamp_bulbs_faulty   : 1; // bit 0
        uint8_t s_rear_lh_tyre_punctured  : 1; // bit 1
        uint8_t s_rear_rh_tyre_punctured  : 1; // bit 2
        uint8_t s_front_rh_tyre_punctured : 1; // bit 3
        uint8_t s_front_lh_tyre_punctured : 1; // bit 4
        uint8_t unknown5                  : 1; // bit 5
        uint8_t unknown6                  : 1; // bit 6
        uint8_t unknown7                  : 1; // bit 7
    } data;
    uint8_t asByte;
};

union CAN_120_2004_Block2Byte3Struct {
    struct
    {
        uint8_t e_rear_right_hand_brake_light_bulb_faulty : 1; // bit 0
        uint8_t e_main_beam_bulbs_faulty1                 : 1; // bit 1
        uint8_t e_main_beam_bulbs_faulty2                 : 1; // bit 2
        uint8_t e_dipped_beam_bulbs_faulty1               : 1; // bit 3
        uint8_t e_dipped_beam_bulbs_faulty2               : 1; // bit 4
        uint8_t e_sidelamp_bulbs_faulty1                  : 1; // bit 5
        uint8_t e_sidelamp_bulbs_faulty2                  : 1; // bit 6
        uint8_t e_sidelamp_bulbs_faulty3                  : 1; // bit 7
    } data;
    uint8_t asByte;
};

union CAN_120_2004_Block2Byte4Struct {
    struct
    {
        uint8_t e_direction_indicators_faulty1           : 1; // bit 0
        uint8_t e_direction_indicators_faulty2           : 1; // bit 1
        uint8_t e_direction_indicators_faulty3           : 1; // bit 2
        uint8_t e_foglamp_bulbs_faulty1                  : 1; // bit 3
        uint8_t e_foglamp_bulbs_faulty2                  : 1; // bit 4
        uint8_t e_foglamp_bulbs_faulty3                  : 1; // bit 5
        uint8_t e_foglamp_bulbs_faulty4                  : 1; // bit 6
        uint8_t e_rear_left_hand_brake_light_bulb_faulty : 1; // bit 7
    } data;
    uint8_t asByte;
};

union CAN_120_2004_Block2Byte5Struct {
    struct
    {
        uint8_t unknown0                                : 1; // bit 0
        uint8_t unknown1                                : 1; // bit 1
        uint8_t unknown2                                : 1; // bit 2
        uint8_t unknown3                                : 1; // bit 3
        uint8_t unknown4                                : 1; // bit 4
        uint8_t e_left_hand_reversing_lamp_bulb_faulty  : 1; // bit 5
        uint8_t e_right_hand_reversing_lamp_bulb_faulty : 1; // bit 6
        uint8_t e_direction_indicators_faulty           : 1; // bit 7
    } data;
    uint8_t asByte;
};

union CAN_120_2004_Block2Byte6Struct {
    struct
    {
        uint8_t e_tyre_pressures_too_low1          : 1; // bit 0
        uint8_t e_tyre_pressures_too_low2          : 1; // bit 1
        uint8_t unknown2                           : 1; // bit 2
        uint8_t unknown3                           : 1; // bit 3
        uint8_t e_parking_assistance_system_faulty : 1; // bit 4
        uint8_t unknown5                           : 1; // bit 5
        uint8_t unknown6                           : 1; // bit 6
        uint8_t unknown7                           : 1; // bit 7
    } data;
    uint8_t asByte;
};

union CAN_120_2004_Block2Byte7Struct {
    struct
    {
        uint8_t unknown0                  : 1; // bit 0
        uint8_t unknown1                  : 1; // bit 1
        uint8_t unknown2                  : 1; // bit 2
        uint8_t unknown3                  : 1; // bit 3
        uint8_t unknown4                  : 1; // bit 4
        uint8_t unknown5                  : 1; // bit 5
        uint8_t e_tyre_pressures_too_low3 : 1; // bit 6
        uint8_t e_tyre_pressures_too_low4 : 1; // bit 7
    } data;
    uint8_t asByte;
};

union CAN_120_2004_Block2Byte8Struct {
    struct
    {
        uint8_t e_rear_left_hand_door_open              : 1; // bit 0
        uint8_t e_rear_right_hand_door_open             : 1; // bit 1
        uint8_t e_front_left_hand_door_open             : 1; // bit 2
        uint8_t e_front_right_hand_door_open            : 1; // bit 3
        uint8_t i_risk_of_ice                             : 1; // bit 4
        uint8_t i_place_automatic_gearbox_in_position_p : 1; // bit 5
        uint8_t unknown6                                : 1; // bit 6
        uint8_t unknown7                                : 1; // bit 7
    } data;
    uint8_t asByte;
};

union CAN120Block2 {
    struct {
        uint8_t placeholder;
        CAN_120_2004_Block2Byte2Struct byte2;
        CAN_120_2004_Block2Byte3Struct byte3;
        CAN_120_2004_Block2Byte4Struct byte4;
        CAN_120_2004_Block2Byte5Struct byte5;
        CAN_120_2004_Block2Byte6Struct byte6;
        CAN_120_2004_Block2Byte7Struct byte7;
        CAN_120_2004_Block2Byte8Struct byte8;
    } data;

    uint64_t asUint64;
    uint8_t raw[8];
};

// Block 3 ---------------------------------------------

union CAN_120_2004_Block3Byte2Struct
{
    struct
    {
        uint8_t unknown0                                  : 1; // bit 0
        uint8_t e_high_speed_check_tyre_pressures_correct : 1; // bit 1
        uint8_t unknown2                                  : 1; // bit 2
        uint8_t unknown3                                  : 1; // bit 3
        uint8_t e_fuel_tank_access_not_properly_locked    : 1; // bit 4
        uint8_t e_rear_screen_open                        : 1; // bit 5
        uint8_t e_bonnet_open                             : 1; // bit 6
        uint8_t e_boot_open                               : 1; // bit 7
    } data;
    uint8_t asByte;
};

union CAN_120_2004_Block3Byte3Struct
{
    struct
    {
        uint8_t unknown0                                              : 1; // bit 0
        uint8_t e_directional_headlamps_faulty                        : 1; // bit 1
        uint8_t s_bonnet_open                                         : 1; // bit 2
        uint8_t e_adjustable_wing_fault_speed_restricted_see_handbook : 1; // bit 3
        uint8_t e_parking_brake_faulty                                : 1; // bit 4
        uint8_t unknown5                                              : 1; // bit 5
        uint8_t unknown6                                              : 1; // bit 6
        uint8_t unknown7                                              : 1; // bit 7
    } data;
    uint8_t asByte;
};

union CAN_120_2004_Block3Byte4Struct
{
    struct
    {
        uint8_t unknown0                      : 1; // bit 0
        uint8_t unknown1                      : 1; // bit 1
        uint8_t e_gearbox_fault_repair_needed : 1; // bit 2
        uint8_t unknown3                      : 1; // bit 3
        uint8_t unknown4                      : 1; // bit 4
        uint8_t unknown5                      : 1; // bit 5
        uint8_t unknown6                      : 1; // bit 6
        uint8_t unknown7                      : 1; // bit 7
    } data;
    uint8_t asByte;
};

union CAN_120_2004_Block3Byte5Struct
{
    struct
    {
        uint8_t unknown0                                                 : 1; // bit 0
        uint8_t unknown1                                                 : 1; // bit 1
        uint8_t e_engine_fault_repair_needed                             : 1; // bit 2
        uint8_t unknown3                                                 : 1; // bit 3
        uint8_t e_tyre_pressures_too_low                                 : 1; // bit 4
        uint8_t unknown5                                                 : 1; // bit 5
        uint8_t e_anti_wander_system_lane_crossing_warning_device_faulty : 1; // bit 6
        uint8_t unknown7                                                 : 1; // bit 7
    } data;
    uint8_t asByte;
};

union CAN_120_2004_Block3Byte6Struct
{
    struct
    {
        uint8_t i_rear_lh_tyre_not_monitored  : 1; // bit 0
        uint8_t i_rear_rh_tyre_not_monitored  : 1; // bit 1
        uint8_t i_front_rh_tyre_not_monitored : 1; // bit 2
        uint8_t i_front_lh_tyre_not_monitored : 1; // bit 3
        uint8_t unknown4                      : 1; // bit 4
        uint8_t unknown5                      : 1; // bit 5
        uint8_t unknown6                      : 1; // bit 6
        uint8_t unknown7                      : 1; // bit 7
    } data;
    uint8_t asByte;
};

union CAN_120_2004_Block3Byte7Struct
{
    struct
    {
        uint8_t unknown0                        : 1; // bit 0
        uint8_t unknown1                        : 1; // bit 1
        uint8_t e_engine_fault_stop_the_vehicle : 1; // bit 2
        uint8_t unknown3                        : 1; // bit 3
        uint8_t unknown4                        : 1; // bit 4
        uint8_t unknown5                        : 1; // bit 5
        uint8_t e_power_steering_faulty         : 1; // bit 6
        uint8_t e_suspension_faulty             : 1; // bit 7
    } data;
    uint8_t asByte;
};

union CAN_120_2004_Block3Byte8Struct
{
    struct
    {
        uint8_t unknown0                  : 1; // bit 0
        uint8_t unknown1                  : 1; // bit 1
        uint8_t unknown2                  : 1; // bit 2
        uint8_t unknown3                  : 1; // bit 3
        uint8_t unknown4                  : 1; // bit 4
        uint8_t unknown5                  : 1; // bit 5
        uint8_t unknown6                  : 1; // bit 6
        uint8_t unknown7                  : 1; // bit 7
    } data;
    uint8_t asByte;
};

union CAN120Block3 {
    struct {
        uint8_t placeholder;
        CAN_120_2004_Block3Byte2Struct byte2;
        CAN_120_2004_Block3Byte3Struct byte3;
        CAN_120_2004_Block3Byte4Struct byte4;
        CAN_120_2004_Block3Byte5Struct byte5;
        CAN_120_2004_Block3Byte6Struct byte6;
        CAN_120_2004_Block3Byte7Struct byte7;
        CAN_120_2004_Block3Byte8Struct byte8;
    } data;

    uint64_t asUint64;
    uint8_t raw[8];
};

struct CMB2004_120Struct {
    CAN_120_2004_Byte1Struct Status;
    uint8_t Field2;
    uint8_t Field3;
    uint8_t Field4;
    uint8_t Field5;
    uint8_t Field6;
    uint8_t Field7;
    uint8_t Field8;
};

union CMB2004_120Packet {
    CMB2004_120Struct data;
    uint8_t CMB2004_120Packet[sizeof(CMB2004_120Struct)];
};
#endif
