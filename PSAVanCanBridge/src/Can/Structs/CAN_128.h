#pragma once

#ifndef _2004_128_h
    #define _2004_128_h

#include <stdint.h>

// CANID: 128
const uint16_t CAN2004_ID_128 = 0x128;
const uint8_t CAN2004_ID_128_LEN = 8;

const uint8_t CAN_DASH_GEAR_P       = 0;
const uint8_t CAN_DASH_GEAR_R       = 1;
const uint8_t CAN_DASH_GEAR_N       = 2;
const uint8_t CAN_DASH_GEAR_D       = 3;
const uint8_t CAN_DASH_GEAR_6       = 4;
const uint8_t CAN_DASH_GEAR_5       = 5;
const uint8_t CAN_DASH_GEAR_4       = 6;
const uint8_t CAN_DASH_GEAR_3       = 7;
const uint8_t CAN_DASH_GEAR_2       = 8;
const uint8_t CAN_DASH_GEAR_1       = 9;
const uint8_t CAN_DASH_GEAR_INVALID = 15;

const uint8_t CAN_DASH_GEAR_MODE_AUTO             = 0;
const uint8_t CAN_DASH_GEAR_MODE_UNUSED1          = 1;
const uint8_t CAN_DASH_GEAR_MODE_AUTO_SPORT       = 2;
const uint8_t CAN_DASH_GEAR_MODE_UNUSED3          = 3;
const uint8_t CAN_DASH_GEAR_MODE_SEQUENTIAL       = 4;
const uint8_t CAN_DASH_GEAR_MODE_SEQUENTIAL_SPORT = 5;
const uint8_t CAN_DASH_GEAR_MODE_AUTO_SNOW        = 6;
const uint8_t CAN_DASH_GEAR_MODE_UNUSED7          = 7;

const uint8_t CAN_DASH_GEAR_SELECTION_BVA    = 0;
const uint8_t CAN_DASH_GEAR_SELECTION_BVM    = 1;
const uint8_t CAN_DASH_GEAR_SELECTION_BVMP   = 2;
const uint8_t CAN_DASH_GEAR_SELECTION_UNUSED = 3;

union Can2004CombineLightsByte1 {
    struct {
        uint8_t service_icon_blink         : 1; // bit 0
        uint8_t passenger_seatbelt_warning : 1; // bit 1
        uint8_t diesel_pre_heating         : 1; // bit 2
        uint8_t fuel_circuit_neutral       : 1; // bit 3
        uint8_t fuel_level_low             : 1; // bit 4
        uint8_t handbrake_signal           : 1; // bit 5
        uint8_t driver_seatbelt_warning    : 1; // bit 6
        uint8_t air_bag_fault              : 1; // bit 7
    } data;
    uint8_t asByte;
};

union Can2004CombineLightsByte2 {
    struct {
        uint8_t rear_seatbelt_warning         : 1; // bit 0
        uint8_t abs_active                    : 1; // bit 1
        uint8_t fuel_flap_open                : 1; // bit 2
        uint8_t open_door_above_10km_per_h    : 1; // bit 3
        uint8_t open_door_below_10km_per_h    : 1; // bit 4
        uint8_t stop_light_blink              : 1; // bit 5
        uint8_t stop_light                    : 1; // bit 6
        uint8_t service_indicator_exclamation : 1; // bit 7
    } data;
    uint8_t asByte;
};

union Can2004CombineLightsByte3 {
    struct {
        uint8_t securoscope_active     : 1; // bit 0
        uint8_t warning_light_active   : 1; // bit 1
        uint8_t suspension_status      : 1; // bit 2
        uint8_t esp_in_progress        : 1; // bit 3
        uint8_t esp_inactivated        : 1; // bit 4
        uint8_t child_security_active  : 1; // bit 5
        uint8_t vehicle_convict_status : 1; // bit 6
        uint8_t convict_while_driving  : 1; // bit 7
    } data;
    uint8_t asByte;
};

typedef struct {
    uint8_t unused1                            : 1; // bit 0
    uint8_t foot_on_break_indicator            : 2; // bit 1-2
    uint8_t operation_indicator_light_blinking : 1; // bit 3
    uint8_t operation_indicator_light_on       : 1; // bit 4
    uint8_t autopark_light                     : 1; // bit 5
    uint8_t unused6                            : 1; // bit 6
    uint8_t seatbelt_forgotten_flashing        : 1; // bit 7
} Can2004CombineLightsByte4;

union Can2004CombineLightsByte5 {
    struct {
        uint8_t unused                  : 1; // bit 0
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

union Can2004CombineLightsByte6 {
    struct {
        uint8_t rear_window_heating_on : 1; // bit 0
        uint8_t unused                 : 3; // bit 1-3
        uint8_t adin_working           : 1; // bit 4
        uint8_t indication_to_use_adin : 1; // bit 5
        uint8_t adin_available         : 1; // bit 6
        uint8_t cmb_active             : 1; // bit 7
    } data;
    uint8_t asByte;
};

union Can2004CombineLightsByte7 {
    struct {
        uint8_t display_blinking : 1; // bit 0
        uint8_t unused           : 3; // bit 1-3
        uint8_t gear_position    : 4; // bit 4-7
    } data;
    uint8_t asByte;
};

union Can2004CombineLightsByte8 {
    struct {
        uint8_t auto_gearbox_selection : 2; // bit 0-1
        uint8_t unused23               : 2; // bit 2-3
        uint8_t auto_gearbox_mode      : 3; // bit 4-6
        uint8_t unused7                : 1; // bit 7
    } data;
    uint8_t asByte;
};

struct CMB2004_128Struct {
    Can2004CombineLightsByte1 Indicator1;
    Can2004CombineLightsByte2 Indicator2;
    Can2004CombineLightsByte3 Indicator3;
    Can2004CombineLightsByte4 Indicator4;
    Can2004CombineLightsByte5 Indicator5;
    Can2004CombineLightsByte6 Indicator6;
    Can2004CombineLightsByte7 Indicator7;
    Can2004CombineLightsByte8 Indicator8;
};

union CMB2004_128Packet {
    CMB2004_128Struct data;
    uint8_t CMB2004_128Packet[sizeof(CMB2004_128Struct)];
};
#endif
