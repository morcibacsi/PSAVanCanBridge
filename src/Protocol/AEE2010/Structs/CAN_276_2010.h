// CAN_276_2010.h
#pragma once

#ifndef _CAN_276_2010_h
    #define _CAN_276_2010_h

#include <stdint.h>

const uint16_t CAN_276_2010_INTERVAL = 1000;

// CANID: 276
const uint16_t CAN_ID_276_2010 = 0x276;

// Read right to left in documentation
union CAN_276_2010_Byte1Struct{
    struct {
        uint8_t year        : 7; // bit 0-6
        uint8_t time_format : 1; // bit 7
    } data;
    uint8_t asByte;
};

union CAN_276_2010_Byte2Struct{
    struct {
        uint8_t month          : 4; // bit 0-3
        uint8_t clock_blinking : 1; // bit 4
        uint8_t lock_status    : 3; // bit 5-7
    } data;
    uint8_t asByte;
};

union CAN_276_2010_Byte3Struct{
    struct {
        uint8_t day          : 6; // bit 0-5
        uint8_t              : 2; // bit 6-7
    } data;
    uint8_t asByte;
};

union CAN_276_2010_Byte4Struct{
    struct {
        uint8_t hour : 5; // bit 0-4
        uint8_t      : 3; // bit 5-7
    } data;
    uint8_t asByte;
};

union CAN_276_2010_Byte5Struct{
    struct {
        uint8_t minute : 6; // bit 0-5
        uint8_t        : 2; // bit 6-7
    } data;
    uint8_t asByte;
};

union CAN_276_2010_BytesStruct {
    struct {
        // BYTE 1: bits 1.0 to 1.7
        // "urea_remaining" takes bits 1.0 through 1.7 (8 bits)
        uint8_t urea_low        : 8;

        // BYTE 2: bits 2.0 to 2.7
        // "urea_remaining" takes bits 2.0 through 2.5 (6 bits)
        // "unused" is at 2.6
        // "request_urea_display" is at 2.7
        uint8_t urea_high       : 6; // bits 2.0 - 2.5
        uint8_t unused          : 1; // bit 2.6
        uint8_t request_display : 1; // bit 2.7
    } field;

    uint8_t  asBytes[2];
    uint16_t asWord;

    // Helper method to get the full 14-bit urea value
    uint16_t getUrea() const {
        return (field.urea_high << 8) | field.urea_low;
    }

    // Helper method to set the full 14-bit urea value
    void setUrea(uint16_t value) {
        field.urea_low = value & 0xFF;
        field.urea_high = (value >> 8) & 0x3F;
    }
};


union CAN_276_2010_Byte6Struct{
    struct
    {
        uint8_t urea_remaining_2     : 6; // bit 0-5
        uint8_t unused               : 1; // bit 6
        uint8_t request_urea_display : 1; // bit 7
    } data;
    uint8_t asByte;
};


// Read left to right in documentation
struct CAN_276_2010_Struct {
    CAN_276_2010_Byte1Struct Year;
    CAN_276_2010_Byte2Struct Month;
    CAN_276_2010_Byte3Struct Day;
    CAN_276_2010_Byte4Struct Hour;
    CAN_276_2010_Byte5Struct Minute;
    uint8_t Unknown6;
    uint8_t Unknown7;
    uint8_t Unknown8;
};
#endif
