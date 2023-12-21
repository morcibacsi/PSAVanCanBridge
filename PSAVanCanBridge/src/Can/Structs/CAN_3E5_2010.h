// CAN_3E5_2010.h
#pragma once

#ifndef _CAN_3E5_2010_h
    #define _CAN_3E5_2010_h

#include <stdint.h>

const uint16_t CAN_3E5_2010_INTERVAL = 500;

// CANID: 3E5
const uint16_t CAN_ID_RADIO_BUTTON_2010 = 0x3E5;

// Read right to left in documentation
union CAN_3E5_2010_Byte1Struct {
    struct {
        uint8_t left       : 1; // bit 0
        uint8_t right      : 1; // bit 1
        uint8_t down       : 1; // bit 2
        uint8_t up         : 1; // bit 3
        uint8_t dark_mode  : 1; // bit 4
        uint8_t mode       : 1; // bit 5
        uint8_t menu       : 1; // bit 6
        uint8_t news       : 1; // bit 7
    }data;
  uint8_t asByte;
};

union CAN_3E5_2010_Byte2Struct {
    struct {
        uint8_t sync_knob  : 1; // bit 0
        uint8_t            : 1; // bit 1
        uint8_t list2      : 1; // bit 2
        uint8_t list       : 1; // bit 3
        uint8_t dark       : 1; // bit 4
        uint8_t phone_menu : 1; // bit 5
        uint8_t esc        : 1; // bit 6
        uint8_t ok         : 1; // bit 7
    }data;
  uint8_t asByte;
};

// Read left to right in documentation
struct CAN_3E5_2010Struct {
    CAN_3E5_2010_Byte1Struct Menu;
    CAN_3E5_2010_Byte2Struct EscOk;
    uint8_t KnobPosition;
};
#endif
