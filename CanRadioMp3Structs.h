// CanRadioMp3Structs.h
#pragma once

#ifndef _CanRadioMp3Structs_h
    #define _CanRadioMp3Structs_h


// CANID: 2E3
const uint16_t CAN_ID_RADIO_MP3 = 0x2E3;

//typedef struct {
//    uint8_t volume       : 5; // bit 0-4
//    uint8_t scrolling    : 3; // bit 5-7
//} CanRadioVolumeByte1Struct;


// Read left to right in documentation
typedef struct CanRadioMp3Struct8 {
    uint8_t Byte1;
    uint8_t Byte2;
    uint8_t Byte3;
    uint8_t Byte4;
    uint8_t Byte5;
    uint8_t Byte6;
    uint8_t Byte7;
    uint8_t Byte8;
};

typedef union CanRadioMp3Packet {
    CanRadioMp3Struct8 data;
    uint8_t CanRadioMp3Packet[sizeof(CanRadioMp3Struct8)];
};

#endif
