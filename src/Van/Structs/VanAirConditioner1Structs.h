// VanAirConditioner1Structs.h
#pragma once

#ifndef _VanAirConditioner1Structs_h
    #define _VanAirConditioner1Structs_h

// VANID: 464
const uint16_t VAN_ID_AIR_CONDITIONER_1 = 0x464;

// Read right to left in documentation
typedef struct {
    uint8_t recycling_button_pressed : 1; // bit 0
    uint8_t unknown1                 : 1; // bit 1
    uint8_t recycling_on             : 1; // bit 2
    uint8_t unknown3                 : 1; // bit 3
    uint8_t aircon_on_if_necessary   : 1; // bit 4 // this means that the car can turn on the A/C if necessary
    uint8_t unknown_6                : 1; // bit 5
    uint8_t auto_mode_on             : 1; // bit 6 // not sure, it is also set, when fan speed is above 3
    uint8_t unknown_8                : 1; // bit 7
} VanAirCon1Byte0Struct;


// Read left to right in documentation
typedef struct VanAirConditioner1Struct {
    VanAirCon1Byte0Struct Status;
    uint8_t Byte1;
    uint8_t Byte2;
    uint8_t Byte3;
    uint8_t FanSpeed;
};

typedef union VanAirConditioner1Packet {
    VanAirConditioner1Struct data;
    uint8_t VanAirConditioner1Packet[sizeof(VanAirConditioner1Struct)];
};

#endif
