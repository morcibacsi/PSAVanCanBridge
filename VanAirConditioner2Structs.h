// VanAirConditioner2Structs.h
#pragma once

#ifndef _VanAirConditioner2Structs_h
    #define _VanAirConditioner2Structs_h

// VANID: 4DC
const uint16_t VAN_ID_AIR_CONDITIONER_2 = 0x4DC;

// Read right to left in documentation
typedef struct {
    uint8_t ac_compressor_running  : 1; // bit 0
    uint8_t unknown1               : 1; // bit 1
    uint8_t unknown2               : 1; // bit 2
    uint8_t unknown3               : 1; // bit 3
    uint8_t unknown4               : 1; // bit 4
    uint8_t rear_window_heating_on : 1; // bit 5
    uint8_t ac_on                  : 1; // bit 6
    uint8_t power_on               : 1; // bit 7
} VanAirCon2Byte0Struct;

// Read right to left in documentation
typedef struct {
    unsigned short data : 16;
} VanAirCon2Byte34Struct;


// Read left to right in documentation
typedef struct VanAirConditioner2Struct {
    VanAirCon2Byte0Struct Status;
    uint8_t Byte1;
    uint8_t InternalTemperature;
    VanAirCon2Byte34Struct EvaporatorTemperature;
    uint8_t Byte5;
};

typedef union VanAirConditioner2Packet {
    VanAirConditioner2Struct data;
    uint8_t VanAirConditioner2Packet[sizeof(VanAirConditioner2Struct)];
};

int8_t GetInternalTemperature(uint8_t byte)
{
    return (byte / 2);
}

int8_t GetEvaporatorTemperature(uint16_t input)
{
    return ((((input & 0xff) << 8) | ((input >> 8) & 0xff)) / 10)-40;
}

#endif
