// VanAirConditioner2Structs.h
#pragma once

#ifndef _VanAirConditioner2Structs_h
    #define _VanAirConditioner2Structs_h


const uint8_t VAN_ID_AIR_CONDITIONER_2_LENGTH = 7;

// VANID: 4DC
const uint16_t VAN_ID_AIR_CONDITIONER_2 = 0x4DC;

// Read right to left in documentation
typedef struct {
    uint8_t ac_compressor_running          : 1; // bit 0
    uint8_t additional_heating_status      : 1; // bit 1
    uint8_t load_shedding_level3_requested : 1; // bit 2
    uint8_t load_shedding_level2_requested : 1; // bit 3
    uint8_t load_shedding_level1_requested : 1; // bit 4
    uint8_t rear_window_heating_on         : 1; // bit 5
    uint8_t ac_on                          : 1; // bit 6
    uint8_t power_on                       : 1; // bit 7
} VanAirCon2Byte1Struct;

typedef struct {
    uint8_t start_stop_engine_status  : 1; // bit 0
    uint8_t cc_roof_status            : 1; // bit 1
    uint8_t safety_status             : 1; // bit 2
    uint8_t ac_management_status      : 2; // bit 3-4
    uint8_t additional_heating_status : 3; // bit 5-7
} VanAirCon2Byte2Struct;

// Read right to left in documentation
typedef struct {
    uint16_t data : 16;
} VanAirCon2Byte34Struct;


// Read left to right in documentation
struct VanAirConditioner2Struct {
    VanAirCon2Byte1Struct Status1;
    VanAirCon2Byte2Struct Status2;
    uint8_t Pressure;
    uint16_t EvaporatorTemperature;
    uint8_t Luminosity1;
    uint8_t Luminosity2;
};

union VanAirConditioner2Packet {
    VanAirConditioner2Struct data;
    uint8_t VanAirConditioner2Packet[sizeof(VanAirConditioner2Struct)];
};

float static GetPressure(uint8_t byte)
{
    return (byte / 20);
}

float static GetEvaporatorTemperature(uint16_t input)
{
    return ((((input & 0xff) << 8) | ((input >> 8) & 0xff)) / 10)-40;
}

#endif
