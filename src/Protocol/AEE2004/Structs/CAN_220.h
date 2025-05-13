#pragma once

#ifndef _2004_220_h
    #define _2004_220_h

#include <stdint.h>

// CANID: 220
const uint16_t CAN2004_ID_220 = 0x220;
const uint8_t CAN2004_ID_220_LEN = 2;

union CAN_220_2004_Byte1Struct {
    struct {
        uint8_t fuel_flap_open        : 1; // bit 0
        uint8_t rear_window_open      : 1; // bit 1
        uint8_t hood_open             : 1; // bit 2
        uint8_t trunk_open            : 1; // bit 3
        uint8_t rear_right_door_open  : 1; // bit 4
        uint8_t rear_left_door_open   : 1; // bit 5
        uint8_t front_right_door_open : 1; // bit 6
        uint8_t front_left_door_open  : 1; // bit 7
    } data;
    uint8_t asByte;
};

union CAN_220_2004_Byte2Struct {
    struct {
        uint8_t                    : 6; // bit 0-5
        uint8_t spare_wheel_status : 1; // bit 6
        uint8_t vehicle_type       : 1; // bit 7
    } data;
    uint8_t asByte;
};

struct Can220_2004_Struct {
    CAN_220_2004_Byte1Struct Field1;
    CAN_220_2004_Byte2Struct Field2;
};

union CMB2004_220Packet {
    Can220_2004_Struct data;
    uint8_t CMB2004_220Packet[sizeof(Can220_2004_Struct)];
};
#endif
