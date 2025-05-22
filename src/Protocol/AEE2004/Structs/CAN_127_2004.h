#pragma once

#ifndef _2004_127_h
    #define _2004_127_h

#include <stdint.h>

// CANID: 127
const uint16_t CAN2004_ID_127 = 0x127;
const uint8_t CAN2004_ID_127_LEN = 3;

union Can2004_127Byte1 {
    struct {
        uint8_t               : 6; // bit 0-5
        uint8_t showroom_mode : 1; // bit 0-6
        uint8_t enable_vth    : 1; // bit 7
    } data;
    uint8_t asByte;
};

struct Can127_2004_Struct {
    Can2004_127Byte1 Status;
    uint8_t Luminosity;
};

union Can2004_127Packet {
    Can127_2004_Struct data;
    uint8_t CMB2004_127Packet[sizeof(Can127_2004_Struct)];
};
#endif
