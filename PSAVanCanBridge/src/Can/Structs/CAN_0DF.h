#pragma once

#ifndef _2004_0DF_h
    #define _2004_0DF_h

#include <stdint.h>

// CANID: 0DF
const uint16_t CAN_ID_0DF = 0x0DF;
const uint8_t CAN_ID_0DF_LEN = 2;

// Read right to left in documentation
union CAN_0DF_Byte1Struct {
    struct {
        uint8_t           : 1; // bit 0
        uint8_t           : 1; // bit 1
        uint8_t           : 1; // bit 2
        uint8_t           : 1; // bit 3
        uint8_t           : 1; // bit 4
        uint8_t           : 1; // bit 5
        uint8_t           : 1; // bit 6
        uint8_t menu_open : 1; // bit 7
    }data;
  uint8_t asByte;
};

// Read right to left in documentation
union CAN_0DF_Byte2Struct {
    struct {
        uint8_t           : 1; // bit 0
        uint8_t           : 1; // bit 1
        uint8_t           : 1; // bit 2
        uint8_t           : 1; // bit 3
        uint8_t           : 1; // bit 4
        uint8_t menu_open : 1; // bit 5
        uint8_t           : 1; // bit 6
        uint8_t           : 1; // bit 7
    }data;
  uint8_t asByte;
};

struct Can0DFStruct {
    CAN_0DF_Byte1Struct Byte1;
    CAN_0DF_Byte2Struct Byte2;
};

union Can0DFPacket {
    Can0DFStruct data;
    uint8_t Can0DFPacket[sizeof(Can0DFStruct)];
};
#endif
