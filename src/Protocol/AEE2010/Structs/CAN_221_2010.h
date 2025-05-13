#pragma once

#ifndef _CAN_221_2010
    #define _CAN_221_2010

#include <stdint.h>

const uint16_t CAN_ID_221_2010_INTERVAL = 1000;

// CANID: 0x361_2010
const uint16_t CAN_ID_221_2010 = 0x221;

// Read right to left in documentation
union CAN_221_2010_Byte1Struct {
    struct {
        uint8_t left_stick_button_pushed          : 1; // bit 0
        uint8_t unused1                           : 1; // bit 1
        uint8_t unused_2                          : 1; // bit 2
        uint8_t right_stick_button_pushed         : 1; // bit 3
        uint8_t unused4                           : 1; // bit 4
        uint8_t steering_wheel_page_scroll_pushed : 1; // bit 5
        uint8_t remaining_range_invalid           : 1; // bit 6
        uint8_t consumption_invalid               : 1; // bit 7
    }data;
  uint8_t asByte;
};

#endif
