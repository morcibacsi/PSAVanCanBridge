#pragma once

#ifndef _2010_126_h
    #define _2010_126_h

#include <stdint.h>

// CANID: 126
const uint16_t CAN2010_ID_126 = 0x126;
const uint8_t CAN2010_ID_126_LEN = 3;

union CAN_126_2010_Byte1Struct {
    struct {
        uint8_t space_measure_request       : 1; // bit 0
        uint8_t hook_present                : 1; // bit 1
        uint8_t trailer_present             : 1; // bit 2
        uint8_t disable_visual              : 1; // bit 3
        uint8_t disable_sound               : 1; // bit 4
        uint8_t space_measurement_available : 1; // bit 5
        uint8_t                             : 2; // bit 6-7
    } data;
    uint8_t asByte;
};

// Read right to left in documentation
union CAN_126_2010_Byte2Struct {
    struct
    {
        uint8_t                   : 4; // bit 0-3
        uint8_t gearbox_selection : 4; // bit 4-7
    } data;
    uint8_t asByte;
};

struct CAN_126_2010_126Struct {
    CAN_126_2010_Byte1Struct Field0;
    CAN_126_2010_Byte2Struct Field1;
    uint8_t Field2;
};

union CAN_126_2010_126Packet {
    CAN_126_2010_126Struct data;
    uint8_t CAN_126_2010_126Packet[sizeof(CAN_126_2010_126Struct)];
};
#endif
