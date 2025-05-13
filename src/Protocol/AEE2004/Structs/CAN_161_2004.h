#pragma once

#ifndef _CAN_161_2004_h
    #define _CAN_161_2004_h

#include <stdint.h>

const uint16_t CAN_161_2004_INTERVAL = 500;

// CANID: 161
const uint16_t CAN_ID_161_2004 = 0x161;

// Read right to left in documentation
union CAN_161_2004_Byte1Struct{
    struct {
        uint8_t unused                      : 7; // bit 0-6
        uint8_t oil_level_restart           : 1; // bit 7
    } data;
    uint8_t asByte;
};


// Read left to right in documentation
struct CAN_161_2004Struct {
    CAN_161_2004_Byte1Struct Field1;
    uint8_t Unused2;
    uint8_t EngineOilTemperature;
    uint8_t FuelLevel;
    uint8_t Unused5;
    uint8_t Unused6;
    uint8_t EngineOilLevel;
};
#endif
