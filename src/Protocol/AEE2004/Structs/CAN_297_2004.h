#pragma once

#ifndef _2004_297_h
    #define _2004_297_h

#include <stdint.h>

// CANID: 297
const uint16_t CAN2004_ID_297 = 0x297;
const uint8_t CAN2004_ID_297_LEN = 2;

const uint8_t VTH_STATUS_INACTIVE = 0;
const uint8_t VTH_STATUS_ACTIVE = 1;
const uint8_t VTH_STATUS_UNAVAILABLE = 2;
const uint8_t VTH_STATUS_INVALID = 4;

union Can2004_297Byte1 {
    struct {
        uint8_t mirror_in_manual_movement : 1; // bit 0
        uint8_t memo_recall               : 1; // bit 1
        uint8_t memorize                  : 1; // bit 2
        uint8_t is_active                 : 2; // bit 4-3
        uint8_t display_fault             : 1; // bit 5
        uint8_t                           : 2; // bit 7-5
    } data;
    uint8_t asByte;
};

struct CAN2004_297Struct {
    Can2004_297Byte1 Status;
    uint8_t SpeedInKmh;
};

union CAN2004_297Packet {
    CAN2004_297Struct data;
    uint8_t CAN2004_297Packet[sizeof(CAN2004_297Struct)];
};
#endif
