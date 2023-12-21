#pragma once

#ifndef _VAN_9C4_h
#define _VAN_9C4_h

#include <stdint.h>

// VANID: 9C4
const uint16_t VAN_ID_REMOTE_STALK = 0x9C4;

// Read right to left in documentation
union Van9C4Byte1Struct {
    struct {
        uint8_t                           : 1; // bit 0
        uint8_t source                : 1; // bit 1
        uint8_t volume_minus              : 1; // bit 2
        uint8_t volume_plus               : 1; // bit 3
        uint8_t counter_overflow_negative : 1; // bit 4
        uint8_t counter_overflow_positive : 1; // bit 5
        uint8_t seek_down                 : 1; // bit 6
        uint8_t seek_up                   : 1; // bit 7
    } data;
    uint8_t asByte;
};

//Read left to right in documentation
struct VanEmfStatusStructs {
    Van9C4Byte1Struct ButtonStatus;
    uint8_t ScroolPosition;
};
#endif