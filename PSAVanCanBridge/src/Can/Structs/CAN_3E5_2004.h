// CAN_3E5_2004_2004.h
#pragma once

#ifndef _CAN_3E5_2004_h
    #define _CAN_3E5_2004_h

#include <stdint.h>

const uint16_t CAN_3E5_2004_INTERVAL = 500;

// CANID: 3E5
const uint16_t CAN_ID_RADIO_BUTTON_2004 = 0x3E5;

// Read right to left in documentation
union CAN_3E5_2004_Byte1Struct {
    struct {
        uint8_t aircon     : 1; // bit 0
        uint8_t            : 1; // bit 1
        uint8_t navigation : 1; // bit 2
        uint8_t            : 1; // bit 3
        uint8_t phone      : 1; // bit 4
        uint8_t            : 1; // bit 5
        uint8_t menu       : 1; // bit 6
        uint8_t            : 1; // bit 7
    }data;
  uint8_t asByte;
};

union CAN_3E5_2004_Byte2Struct {
    struct {
        uint8_t audio      : 1; // bit 0
        uint8_t            : 1; // bit 1
        uint8_t main       : 1; // bit 2
        uint8_t            : 1; // bit 3
        uint8_t mode       : 1; // bit 4
        uint8_t            : 1; // bit 5
        uint8_t odb        : 1; // bit 6
        uint8_t            : 1; // bit 7
    }data;
  uint8_t asByte;
};

union CAN_3E5_2004_Byte3Struct {
    struct {
        uint8_t            : 1; // bit 0
        uint8_t            : 1; // bit 1
        uint8_t dark       : 1; // bit 2
        uint8_t            : 1; // bit 3
        uint8_t esc        : 1; // bit 4
        uint8_t            : 1; // bit 5
        uint8_t ok         : 1; // bit 6
        uint8_t            : 1; // bit 7
    }data;
  uint8_t asByte;
};

union CAN_3E5_2004_Byte4Struct {
    struct {
        uint8_t                        : 1; // bit 0
        uint8_t                        : 1; // bit 1
        uint8_t                        : 1; // bit 2
        uint8_t                        : 1; // bit 3
        uint8_t overflow_scan_negative : 1; // bit 4
        uint8_t                        : 1; // bit 5
        uint8_t overflow_scan_positive : 1; // bit 6
        uint8_t                        : 1; // bit 7
    }data;
  uint8_t asByte;
};

union CAN_3E5_2004_Byte6Struct {
    struct {
        uint8_t left  : 1; // bit 0
        uint8_t       : 1; // bit 1
        uint8_t right : 1; // bit 2
        uint8_t       : 1; // bit 3
        uint8_t down  : 1; // bit 4
        uint8_t       : 1; // bit 5
        uint8_t up    : 1; // bit 6
        uint8_t       : 1; // bit 7
    }data;
  uint8_t asByte;
};

// Read left to right in documentation
struct CAN_3E5_2004Struct {
    CAN_3E5_2004_Byte1Struct Menu;
    CAN_3E5_2004_Byte2Struct Mode;
    CAN_3E5_2004_Byte3Struct DarkEscOk;
    CAN_3E5_2004_Byte4Struct Overflow;
    uint8_t ScanWheelCounter;
    CAN_3E5_2004_Byte6Struct Directions;
};
#endif
