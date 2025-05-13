#pragma once

#ifndef _2010_128_h
    #define _2010_128_h

#include <stdint.h>

// CANID: 128
const uint16_t CAN2010_ID_128 = 0x128;
const uint8_t CAN2010_ID_128_LEN = 8;

const uint8_t CAN_2010_DASH_GEAR_P       = 0;
const uint8_t CAN_2010_DASH_GEAR_R       = 1;
const uint8_t CAN_2010_DASH_GEAR_N       = 2;
const uint8_t CAN_2010_DASH_GEAR_D       = 3;
const uint8_t CAN_2010_DASH_GEAR_6       = 4;
const uint8_t CAN_2010_DASH_GEAR_5       = 5;
const uint8_t CAN_2010_DASH_GEAR_4       = 6;
const uint8_t CAN_2010_DASH_GEAR_3       = 7;
const uint8_t CAN_2010_DASH_GEAR_2       = 8;
const uint8_t CAN_2010_DASH_GEAR_1       = 9;
const uint8_t CAN_2010_DASH_GEAR_INVALID = 10;
const uint8_t CAN_2010_DASH_GEAR_NONE    = 11;
const uint8_t CAN_2010_DASH_GEAR_7       = 12;
const uint8_t CAN_2010_DASH_GEAR_8       = 13;
const uint8_t CAN_2010_DASH_GEAR_9       = 14;
const uint8_t CAN_2010_DASH_GEAR_B       = 15;

const uint8_t CAN_2010_DASH_GEAR_MODE_AUTO             = 0;
const uint8_t CAN_2010_DASH_GEAR_MODE_UNUSED1          = 1;
const uint8_t CAN_2010_DASH_GEAR_MODE_AUTO_SPORT       = 2;
const uint8_t CAN_2010_DASH_GEAR_MODE_UNUSED3          = 3;
const uint8_t CAN_2010_DASH_GEAR_MODE_SEQUENTIAL       = 4;
const uint8_t CAN_2010_DASH_GEAR_MODE_SEQUENTIAL_SPORT = 5;
const uint8_t CAN_2010_DASH_GEAR_MODE_AUTO_SNOW        = 6;
const uint8_t CAN_2010_DASH_GEAR_MODE_ASM              = 7;

const uint8_t CAN_2010_DASH_GEAR_SELECTION_BVA    = 0;
const uint8_t CAN_2010_DASH_GEAR_SELECTION_BVM    = 1;
const uint8_t CAN_2010_DASH_GEAR_SELECTION_BVMP   = 2;
const uint8_t CAN_2010_DASH_GEAR_SELECTION_UNUSED = 3;

union Can2010CombineLightsByte1 {
    struct {
        uint8_t drl                     : 1; // bit 0
        uint8_t left_turn_indicator     : 1; // bit 1
        uint8_t right_turn_indicator    : 1; // bit 2
        uint8_t rear_foglight           : 1; // bit 3
        uint8_t front_foglight          : 1; // bit 4
        uint8_t high_beam_on            : 1; // bit 5
        uint8_t low_beam_on             : 1; // bit 6
        uint8_t parking_light_indicator : 1; // bit 7
    } data;
    uint8_t asByte;
};

union Can2010CombineLightsByte2 {
    struct {
        uint8_t display_blinking         : 1; // bit 0
        uint8_t gear_position_in_driving : 3; // bit 1-3
        uint8_t gear_position            : 4; // bit 4-7
    } data;
    uint8_t asByte;
};

union Can2010CombineLightsByte3 {
    struct {
        uint8_t auto_gearbox_selection     : 2; // bit 0-1
        uint8_t recommended_gear_direction : 2; // bit 2-3
        uint8_t auto_gearbox_mode          : 3; // bit 4-6
        uint8_t recommended_gear_blinking  : 1; // bit 7
    } data;
    uint8_t asByte;
};

union Can2010CombineLightsByte4 {
    struct {
        uint8_t fse_inhibited                 : 1; // bit 0
        uint8_t handbrake_signal              : 1; // bit 1
        uint8_t foot_on_break_indicator       : 2; // bit 2-3
        uint8_t passenger_airbag_activated    : 1; // bit 4
        uint8_t child_security_active         : 1; // bit 5
        uint8_t stop_light                    : 1; // bit 6
        uint8_t service_indicator_exclamation : 1; // bit 7
    } data;
    uint8_t asByte;
};

union Can2010CombineLightsByte5 {
    struct {
        uint8_t suspension_status                  : 1; // bit 0
        uint8_t esp_in_progress                    : 1; // bit 1
        uint8_t esp_inactivated                    : 1; // bit 2
        uint8_t cafr_indicator                     : 1; // bit 3
        uint8_t operation_indicator_light_blinking : 1; // bit 4
        uint8_t operation_indicator_light_on       : 1; // bit 5
        uint8_t door_open                          : 1; // bit 6
        uint8_t diesel_pre_heating                 : 1; // bit 7
    } data;
    uint8_t asByte;
};

union Can2010CombineLightsByte6 {
    struct {
        uint8_t row1_rl_seatbelt_forgotten          : 1; // bit 0
        uint8_t scr_indicator                       : 2; // bit 1-2
        uint8_t passenger_seatbelt_warning_blinking : 1; // bit 3
        uint8_t passenger_seatbelt_warning          : 1; // bit 4
        uint8_t driver_seatbelt_warning_blinking    : 1; // bit 5
        uint8_t driver_seatbelt_warning             : 1; // bit 6
        uint8_t fuel_level_low                      : 1; // bit 7
    } data;
    uint8_t asByte;
};

union Can2010CombineLightsByte7 {
    struct {
        uint8_t activate_front_passenger_protection : 1; // bit 0
        uint8_t warning_light_active                : 1; // bit 1
        uint8_t cmb_active                          : 1; // bit 2
        uint8_t row1_rr_seatbelt_forgotten_blinking : 1; // bit 3
        uint8_t row1_rr_seatbelt_forgotten          : 1; // bit 4
        uint8_t row1_rc_seatbelt_forgotten_blinking : 1; // bit 5
        uint8_t row1_rc_seatbelt_forgotten          : 1; // bit 6
        uint8_t row1_rl_seatbelt_forgotten_blinking : 1; // bit 7
    } data;
    uint8_t asByte;
};

union Can2010CombineLightsByte8 {
    struct {
        uint8_t change_personalization : 1; // bit 0
        uint8_t change_color           : 1; // bit 1
        uint8_t ready_lamp             : 1; // bit 2
        uint8_t minc_blinking          : 1; // bit 3
        uint8_t foot_on_clutch         : 2; // bit 5-4
        uint8_t engine_indicator       : 2; // bit 7-6
    } data;
    uint8_t asByte;
};

struct CMB2010_128Struct {
    Can2010CombineLightsByte1 Indicator1;
    Can2010CombineLightsByte2 Indicator2;
    Can2010CombineLightsByte3 Indicator3;
    Can2010CombineLightsByte4 Indicator4;
    Can2010CombineLightsByte5 Indicator5;
    Can2010CombineLightsByte6 Indicator6;
    Can2010CombineLightsByte7 Indicator7;
    Can2010CombineLightsByte8 Indicator8;
};

union CMB2010_128Packet {
    CMB2010_128Struct data;
    uint8_t CMB2010_128Packet[sizeof(CMB2010_128Struct)];
};
#endif
