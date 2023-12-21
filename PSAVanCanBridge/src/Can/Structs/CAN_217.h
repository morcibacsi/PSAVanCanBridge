// CAN_217.h
#pragma once

#ifndef _CAN_217_h
    #define _CAN_217_h

#include <stdint.h>

const uint16_t CAN_217_INTERVAL = 100;

// CANID: 217
const uint16_t CAN_ID_217 = 0x217;

// Read right to left in documentation
union CAN_217_Byte1Struct {
    struct {
        uint8_t airbag_disabled    : 2; // bit 0-1
        uint8_t reset_maintenance  : 1; // bit 2
        uint8_t black_panel        : 1; // bit 3
        uint8_t brightness         : 4; // bit 4-7
    }data;
  uint8_t asByte;
};

// Read right to left in documentation
union CAN_217_Byte2Struct {
    struct {
        uint8_t unknown0    : 1; // bit 0
        uint8_t reset_trip  : 1; // bit 1
        uint8_t unknown2    : 1; // bit 2
        uint8_t unknown3    : 1; // bit 3
        uint8_t unknown4    : 1; // bit 4
        uint8_t unknown5    : 1; // bit 5
        uint8_t unknown6    : 1; // bit 6
        uint8_t unknown7    : 1; // bit 7
    }data;
  uint8_t asByte;
};

// Read left to right in documentation
struct CAN_217Struct {
    CAN_217_Byte1Struct Brightness;
    CAN_217_Byte2Struct Field2;
    uint8_t Field3;
    uint8_t Field4;
    uint8_t Field5;
    uint8_t Field6;
    uint8_t Field7;
    uint8_t Field8;
};
#endif
