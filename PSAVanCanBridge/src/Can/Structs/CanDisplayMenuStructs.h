// CanDisplayMenuStructs.h
#pragma once

#ifndef _CanDisplayMenuStructs_h
    #define _CanDisplayMenuStructs_h


// CANID: 0DF
const uint16_t CAN_ID_DISPLAY_MENU = 0x0DF;

typedef struct {
    uint8_t           : 1; // bit 0
    uint8_t           : 1; // bit 1
    uint8_t           : 1; // bit 2
    uint8_t           : 1; // bit 3
    uint8_t           : 1; // bit 4
    uint8_t           : 1; // bit 5
    uint8_t           : 1; // bit 6
    uint8_t menu_open : 1; // bit 7
} CanDisplayMenuStructByte1;

typedef struct {
    uint8_t           : 1; // bit 0
    uint8_t           : 1; // bit 1
    uint8_t           : 1; // bit 2
    uint8_t           : 1; // bit 3
    uint8_t           : 1; // bit 4
    uint8_t menu_open : 1; // bit 5
    uint8_t           : 1; // bit 6
    uint8_t           : 1; // bit 7
} CanDisplayMenuStructByte2;


// Read left to right in documentation
struct CanDisplayMenuStruct {
    CanDisplayMenuStructByte1 Byte1;
    uint8_t Byte2;
    CanDisplayMenuStructByte2 Byte3;
};

union CanDisplayMenuPacket {
    CanDisplayMenuStruct data;
    uint8_t CanDisplayMenuPacket[sizeof(CanDisplayMenuStruct)];
};

#endif
