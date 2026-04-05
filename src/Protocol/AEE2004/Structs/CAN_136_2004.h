#pragma once

#ifndef _2004_136_h
    #define _2004_136_h

#include <stdint.h>

// CANID: 136
const uint16_t CAN2004_ID_136 = 0x136;
const uint8_t CAN2004_ID_136_LEN = 2;

union CAN_136_2004_BytesStruct {
    // 1. The bitfield representation
    struct {
        uint8_t  unused               : 1;  // bit 0
        uint8_t  request_urea_display : 1;  // bit 1
        uint16_t urea_remaining       : 14; // bit 2-15
    } field;

    // 2. The full 16-bit value
    uint16_t asWord;

    // 3. The individual bytes
    struct {
        uint8_t lowByte;  // Bits 0-7
        uint8_t highByte; // Bits 8-15
    } bytes;
};

struct CAN_136_2004_136Struct {
    uint8_t UreaRemaining1;
    uint8_t UreaRemaining2_RequestDisplay;
};

union CAN_136_2004_136Packet {
    CAN_136_2004_136Struct data;
    uint8_t CAN_136_2004_136Packet[sizeof(CAN_136_2004_136Struct)];
};
#endif
