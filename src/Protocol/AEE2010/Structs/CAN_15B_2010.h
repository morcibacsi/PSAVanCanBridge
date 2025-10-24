// CAN_15B_2010.h
#pragma once

#ifndef _CAN_15B_2010_h
    #define _CAN_15B_2010_h

#include <stdint.h>

const uint16_t CAN_15B_2010_INTERVAL = 1000;

// CANID: 15B
const uint16_t CAN_ID_15B_2010 = 0x15B;

// Read right to left in documentation
union CAN_15B_2010_Byte1Struct{
    struct {
        uint8_t consumption_unit : 1; // bit 0
        uint8_t distance_unit    : 1; // bit 1
        uint8_t language         : 5; // bit 2-6
        uint8_t data_valid       : 1; // bit 7
    } data;
    uint8_t asByte;
};

union CAN_15B_2010_Byte2Struct{
    struct {
        uint8_t sound_harmony    : 2; // bit 0-1
        uint8_t data_valid       : 1; // bit 2
        uint8_t ambience_level   : 3; // bit 3-5
        uint8_t temperature_unit : 1; // bit 6
        uint8_t volume_unit      : 1; // bit 7
    } data;
    uint8_t asByte;
};

union CAN_15B_2010_Byte3Struct{
    struct {
        uint8_t ambience_light                 : 1; // bit 0
        uint8_t daylight_running_light_enabled : 1; // bit 1
        uint8_t adaptive_lights                : 1; // bit 2
        uint8_t driver_welcome_interior        : 1; // bit 3
        uint8_t selective_trunk_openings       : 1; // bit 4
        uint8_t selective_front_door_openings  : 1; // bit 5
        uint8_t selective_opening_key          : 1; // bit 6
        uint8_t automatic_parking_brake        : 1; // bit 7
    } data;
    uint8_t asByte;
};

union CAN_15B_2010_Byte4Struct{
    struct {
        uint8_t automatic_headlights_enabled : 1; // bit 0
        uint8_t unlock_light_time            : 2; // bit 1-2 //0: 15 seconds, 1: 30 seconds, 2: 60 seconds
        uint8_t unlock_light_enabled         : 1; // bit 3
        uint8_t highway_beam_enabled         : 1; // bit 4
        uint8_t follow_me_home_time          : 2; // bit 5-6 //0: 15 seconds, 1: 30 seconds, 2: 60 seconds
        uint8_t follow_me_home_enabled       : 1; // bit 7
    } data;
    uint8_t asByte;
};

union CAN_15B_2010_Byte5Struct{
    struct {
        uint8_t configurable_button_function  : 4; // bit 0-3
        uint8_t unknown4                      : 1; // bit 4
        uint8_t auto_rear_wiper_enabled       : 1; // bit 5
        uint8_t blind_spot_monitoring_enabled : 1; // bit 6
        uint8_t aas_enabled                   : 1; // bit 7
    } data;
    uint8_t asByte;
};

union CAN_15B_2010_Byte6Struct{
    struct {
        uint8_t etsr_enabled    : 1; // bit 0
        uint8_t see_enabled     : 1; // bit 1
        uint8_t unknown2        : 1; // bit 2
        uint8_t dsg_enabled     : 1; // bit 3
        uint8_t brake_on_alarm  : 1; // bit 4
        uint8_t arc_sensitivity : 2; // bit 5-6
        uint8_t arc_enabled     : 1; // bit 7
    } data;
    uint8_t asByte;
};

union CAN_15B_2010_Byte7Struct{
    struct {
        uint8_t driver_alert_active : 1; // bit 0
        uint8_t unknown1            : 1; // bit 1
        uint8_t unknown2            : 1; // bit 2
        uint8_t unknown3            : 1; // bit 3
        uint8_t unknown4            : 1; // bit 4
        uint8_t cafr_enabled        : 1; // bit 5
        uint8_t profile_number      : 2; // bit 6-7
    } data;
    uint8_t asByte;
};

union CAN_15B_2010_Byte8Struct{
    struct {
        uint8_t amla        : 1; // bit 0
        uint8_t afs         : 1; // bit 1
        uint8_t ars         : 1; // bit 2
        uint8_t irv         : 1; // bit 3
        uint8_t dae_mode_4w : 1; // bit 4-5
        uint8_t dae_mode    : 2; // bit 6-7
    } data;
    uint8_t asByte;
};

// Read left to right in documentation
struct CAN_15B_2010_Struct {
    CAN_15B_2010_Byte1Struct Field1;
    CAN_15B_2010_Byte2Struct Field2;
    CAN_15B_2010_Byte3Struct Field3;
    CAN_15B_2010_Byte4Struct Field4;
    CAN_15B_2010_Byte5Struct Field5;
    CAN_15B_2010_Byte6Struct Field6;
    CAN_15B_2010_Byte7Struct Field7;
    CAN_15B_2010_Byte8Struct Field8;
};
#endif
