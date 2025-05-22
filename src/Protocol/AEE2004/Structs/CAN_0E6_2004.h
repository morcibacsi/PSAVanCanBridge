#pragma once

#ifndef _2004_0E6_h
    #define _2004_0E6_h

#include <stdint.h>

// CANID: 0E6
const uint16_t CAN_ID_0E6 = 0x0E6;
const uint8_t CAN_ID_0E6_LEN = 5;

// Read right to left in documentation
union CAN_0E6_Byte1Struct {
    struct {
        uint8_t abr_fault                 : 1; // bit 0
        uint8_t brake_fluid_level_alert   : 1; // bit 1
        uint8_t brake_pads_worn           : 1; // bit 2
        uint8_t slip_alert                : 2; // bit 3-4
        uint8_t abs_in_progress           : 1; // bit 5
        uint8_t                           : 1; // bit 6
        uint8_t ree_fault                 : 1; // bit 7
    }data;
  uint8_t asByte;
};

// Read right to left in documentation
union CAN_0E6_Byte23Struct {
    struct {
        uint16_t counter_value            : 15; // bit 0-15
        uint8_t rear_left_counter_failure : 1; // bit 16
    }data;
  uint16_t as2Byte;
};

struct Can0E6Struct {
    CAN_0E6_Byte1Struct  Byte1;
    CAN_0E6_Byte23Struct RearLeftWheel;
    CAN_0E6_Byte23Struct RearRightWheel;
};

union Can0E6Packet {
    Can0E6Struct data;
    uint8_t Can0E6Packet[sizeof(Can0E6Struct)];
};
#endif
