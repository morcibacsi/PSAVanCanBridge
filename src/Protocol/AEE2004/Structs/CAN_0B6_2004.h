#pragma once

#ifndef _CAN_0B6_2004_h
    #define _CAN_0B6_2004_h

#include <stdint.h>

// CANID: 0B6
const uint16_t CAN_ID_0B6 = 0x0B6;
const uint8_t CAN_ID_0B6_LEN = 8;

// Read right to left in documentation
union CAN_0B6_Byte8Struct {
    struct {
        uint8_t unused           : 3; // bit 0-2
        uint8_t info_valid_value : 4; // bit 3-6
        uint8_t info_valid       : 1; // bit 7
    }data;
  uint8_t asByte;
};

struct Can0B6Struct {
    uint8_t RpmByte1;
    uint8_t RpmByte2;
    uint8_t SpeedByte1;
    uint8_t SpeedByte2;
    uint8_t DistanceForCMB1;
    uint8_t DistanceForCMB2;
    uint8_t ConsumptionForCMB;
    CAN_0B6_Byte8Struct Field8;
};

union Can0B6Packet {
    Can0B6Struct data;
    uint8_t Can0B6Packet[sizeof(Can0B6Struct)];
};
#endif
