#pragma once

#ifndef _2004_126_h
    #define _2004_126_h

#include <stdint.h>

// CANID: 126
const uint16_t CAN2004_ID_126 = 0x126;
const uint8_t CAN2004_ID_126_LEN = 3;

union Can2004AASByte1 {
    struct {
        uint8_t activate_measurements : 1; // bit 0
        uint8_t hook_present          : 1; // bit 1
        uint8_t trailer_present       : 1; // bit 2
        uint8_t visual_request        : 1; // bit 3
        uint8_t sound_request         : 1; // bit 4
        uint8_t                       : 3; // bit 5-7
    } data;
    uint8_t asByte;
};

struct CMB2004_126Struct {
    Can2004AASByte1 Status;
    uint8_t Unused1;
    uint8_t Unused2;
};

union CMB2004_126Packet {
    CMB2004_126Struct data;
    uint8_t CMB2004_126Packet[sizeof(CMB2004_126Struct)];
};
#endif
