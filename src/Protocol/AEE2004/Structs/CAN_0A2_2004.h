#pragma once

#ifndef _CAN_0A2_2004_h
    #define _CAN_0A2_2004_h

#include <stdint.h>

const uint16_t CAN_0A2_2004_INTERVAL = 100;
const uint16_t CAN_ID_0A2_2004 = 0x0A2;

union CAN_0A2_2004_Byte2Struct {
    struct {
        uint8_t voice_command       : 1; // bit 0
        uint8_t left_push           : 1; // bit 1
        uint8_t mode_button         : 1; // bit 2
        uint8_t menu_button         : 1; // bit 3
        uint8_t escape_button       : 1; // bit 4
        uint8_t validation_button   : 1; // bit 5
        uint8_t scroll_underflow    : 1; // bit 6
        uint8_t scroll_overflow     : 1; // bit 7
    } data;
    uint8_t asByte;
};

union CAN_0A2_2004_Byte3Struct {
    struct {
        uint8_t                   : 7; // bit 0-6
        uint8_t telephone_command : 1; // bit 7
    } data;
    uint8_t asByte;
};

union CAN_0A2_2004_Byte5Struct {
    struct {

        uint8_t volume_knob_underflow : 1; // bit 0
        uint8_t volume_knob_overflow  : 1; // bit 1
        uint8_t                       : 1; // bit 2
        uint8_t ac_recycling_command  : 1; // bit 3
        uint8_t cmb_escape_button     : 1; // bit 4
        uint8_t cmb_validation        : 1; // bit 5
        uint8_t cmb_underflow         : 1; // bit 6
        uint8_t cmb_overflow          : 1; // bit 7
    } data;
    uint8_t asByte;
};

struct CAN_0A2_2004Struct {
    uint8_t NavMenuScroll;
    CAN_0A2_2004_Byte2Struct NavMenuCommands;
    CAN_0A2_2004_Byte3Struct PhoneCommands;
    uint8_t CmbNavMenuCounter;
    CAN_0A2_2004_Byte5Struct CmbCommands;
};
#endif
