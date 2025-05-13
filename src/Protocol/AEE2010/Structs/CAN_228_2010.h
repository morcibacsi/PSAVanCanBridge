// CAN_228_2010.h
#pragma once

#ifndef _CAN_228_2010_h
    #define _CAN_228_2010_h

#include <stdint.h>

const uint16_t CAN_228_2010_INTERVAL = 1000;

// CANID: 228
const uint16_t CAN_ID_228_2010 = 0x228;

constexpr uint8_t SETTING_STATUS_NO_ADJUSTMENT = 0;
constexpr uint8_t SETTING_STATUS_ADJUSTMENT = 1;

// Read right to left in documentation
union CAN_228_2010_Byte3Struct{
    struct {
        uint8_t setting_status              : 1; // bit 0
        uint8_t target_present              : 1; // bit 1
        uint8_t activate_function           : 1; // bit 2
        uint8_t status_of_selected_function : 3; // bit 3-5
        uint8_t selected_function           : 2; // bit 6-7
    } data;
    uint8_t asByte;
};

union CAN_228_2010_Byte4Struct{
    struct {
        uint8_t TIV_time                : 6; // bit 0-5
        uint8_t adjustment_in_progress  : 1; // bit 6
        uint8_t available               : 1; // bit 7
    } data;
    uint8_t asByte;
};

union CAN_228_2010_Byte5Struct{
    struct {
        uint8_t TIV_time                : 6; // bit 0-5
        uint8_t page_requested          : 2; // bit 6-7
    } data;
    uint8_t asByte;
};

// Read left to right in documentation
struct CAN_228_2010_Struct {
    uint8_t SpeedByte1;
    uint8_t SpeedByte2;
    CAN_228_2010_Byte3Struct Status1;
    CAN_228_2010_Byte4Struct Status2;
    CAN_228_2010_Byte5Struct Status3;
    uint8_t Unknown6;
    uint8_t Unknown7;
    uint8_t Unknown8;
};
#endif
