// CAN_1A8_2004.h
#pragma once

#ifndef _CAN_1A8_2004_h
    #define _CAN_1A8_2004_h

#include <stdint.h>

const uint16_t CAN_1A8_2004_INTERVAL = 1000;

// CANID: 1A8
const uint16_t CAN_ID_1A8_2004 = 0x1A8;

// Read right to left in documentation
union CAN_1A8_2004_Byte1Struct{
    struct {
        uint8_t setting_status              : 1; // bit 0
        uint8_t unit_of_speed               : 1; // bit 1
        uint8_t activate_function           : 1; // bit 2
        uint8_t status_of_selected_function : 3; // bit 3-5
        uint8_t selected_function           : 2; // bit 6-7
    } data;
    uint8_t asByte;
};


// Read left to right in documentation
struct CAN_1A8_2004Struct {
    CAN_1A8_2004_Byte1Struct Field1;
    uint8_t CruiseControlSpeed1;
    uint8_t CruiseControlSpeed2;
    uint8_t Field4;
    uint8_t Field5;
    uint8_t TripOnCMB1;
    uint8_t TripOnCMB2;
    uint8_t TripOnCMB3;
};
#endif
