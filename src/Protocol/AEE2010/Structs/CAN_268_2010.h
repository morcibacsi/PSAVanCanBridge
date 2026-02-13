// CAN_268_2010.h
#pragma once

#ifndef _CAN_268_2010_h
    #define _CAN_268_2010_h
#include <stdint.h>

const uint16_t CAN_268_2010_INTERVAL = 1000;

// CANID: 268
const uint16_t CAN_ID_268_2010 = 0x268;

// Read right to left in documentation
union CAN_268_2010_Byte2Struct{
    struct {
        uint8_t poi                        : 4; // bit 0-3
        uint8_t speed_info_high_confidence : 1; // bit 4
        uint8_t overspeed                  : 1; // bit 5
        uint8_t roadsign_type              : 2; // bit 6-7
    } data;
    uint8_t asByte;
};

// Read left to right in documentation
struct CAN_268_2010_Struct {
    uint8_t Speed;
    CAN_268_2010_Byte2Struct SpeedOptions;
    uint8_t Unknown3;
    uint8_t Unknown4;
    uint8_t Unknown5;
    uint8_t Unknown6;
    uint8_t Unknown7;
    uint8_t Unknown8;
};
#endif
