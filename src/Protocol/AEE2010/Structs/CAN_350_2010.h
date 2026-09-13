#pragma once

#ifndef _Can350_2010Struct_h
    #define _Can350_2010Struct_h

#include <stdint.h>

const uint16_t CAN_ID_350_2010_INTERVAL = 500;

// CANID: 0x350_2010
const uint16_t CAN_ID_350_2010 = 0x350;

// Read right to left in documentation
union Can350_2010Byte1Struct {
    struct {
        uint8_t ac_type                      : 2; // bit 0-1
        uint8_t ac_request                   : 2; // bit 2-3
        uint8_t visibility_request           : 1; // bit 4
        uint8_t rear_window_heating_status   : 1; // bit 5
        uint8_t rest_function_status         : 1; // bit 6
        uint8_t fr_pulser_motor_fault        : 1; // bit 7
    }data;
  uint8_t asByte;
};

// Read right to left in documentation
union Can350_2010Byte4Struct {
    struct {
        uint8_t front_left_temperature   : 5; // bit 0-4
        uint8_t flag_restore_active      : 1; // bit 5
        uint8_t mono_mode_active         : 1; // bit 6
        uint8_t ac_max_active            : 1; // bit 7
    }data;
  uint8_t asByte;
};

// Read right to left in documentation
union Can350_2010Byte5Struct {
    struct {
        uint8_t front_right_temperature    : 5; // bit 0-4
        uint8_t front_left_seat_fan_speed  : 2; // bit 5-6
        uint8_t windscreen_heating_active  : 1; // bit 7
    }data;
  uint8_t asByte;
};

// Read right to left in documentation
union Can350_2010Byte6Struct {
    struct {
        uint8_t fan_speed              : 4; // bit 0-3
        uint8_t air_inlet              : 3; // bit 4-6
        uint8_t aqs_active             : 1; // bit 7
    }data;
  uint8_t asByte;
};

// Read right to left in documentation
union Can350_2010Byte7Struct {
    struct {
        uint8_t front_right_distribution        : 4; // bit 3
        uint8_t front_left_distribution         : 4; // bit 4-7
    }data;
  uint8_t asByte;
};

// Read right to left in documentation
union Can350_2010Byte8Struct {
    struct {
        uint8_t                             : 1; // bit 0
        uint8_t front_right_seat_fan_speed  : 2; // bit 1-2
        uint8_t front_left_seat_heat_level  : 2; // bit 3-4
        uint8_t front_right_seat_heat_level : 2; // bit 5-6
        uint8_t energy_saving_mode_active   : 1; // bit 7
    }data;
  uint8_t asByte;
};

// Read left to right in documentation
struct Can350_2010Struct {
    Can350_2010Byte1Struct Field1;
    uint8_t EvaporatorTemperature1;
    uint8_t EvaporatorTemperature2;
    Can350_2010Byte4Struct Field4;
    Can350_2010Byte5Struct Field5;
    Can350_2010Byte6Struct Field6;
    Can350_2010Byte7Struct Field7;
    Can350_2010Byte8Struct Field8;
};
#endif
