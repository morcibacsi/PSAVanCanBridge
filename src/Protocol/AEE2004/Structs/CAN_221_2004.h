#pragma once

#ifndef _CAN_221_2004
    #define _CAN_221_2004

#include <stdint.h>

const uint16_t CAN_ID_221_2004_INTERVAL = 1000;

const uint16_t CAN_ID_221_2004 = 0x221;

// Read right to left in documentation
union CAN_221_2004_Byte1Struct {
    struct {
        uint8_t left_stick_button_pushed          : 1; // bit 0
        uint8_t unused1                           : 1; // bit 1
        uint8_t unused2                           : 1; // bit 1
        uint8_t right_stick_button_pushed         : 1; // bit 3
        uint8_t unused4                           : 1; // bit 4
        uint8_t unused5                           : 1; // bit 5
        uint8_t remaining_range_invalid           : 1; // bit 6
        uint8_t consumption_invalid               : 1; // bit 7
    }data;
  uint8_t asByte;
};

struct CAN_221_2004_Struct {
    CAN_221_2004_Byte1Struct Field1;
    uint8_t InstantConsumptionByte1;
    uint8_t InstantConsumptionByte2;
    uint8_t RemainingRangeByte1;
    uint8_t RemainingRangeByte2;
    uint8_t TotalRangeByte1;
    uint8_t TotalRangeByte2;
};

#endif
