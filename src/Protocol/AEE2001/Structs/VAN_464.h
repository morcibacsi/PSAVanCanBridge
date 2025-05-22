// VanAirConditioner1Structs.h
#pragma once

#ifndef _VAN_464_h
    #define _VAN_464_h

#include <stdint.h>

const uint8_t VAN_ID_AIR_CONDITIONER_1_LENGTH = 5;

// VANID: 464
const uint16_t VAN_ID_AIR_CONDITIONER_1 = 0x464;

// Read right to left in documentation
union VanAirCon1Byte0Struct {
    struct {
        uint8_t heated_screen_active          : 1; // bit 0
        uint8_t electric_windshield_active    : 1; // bit 1
        uint8_t recycling_on                  : 1; // bit 2
        uint8_t aircon_request_valid          : 1; // bit 3
        uint8_t aircon_requested              : 1; // bit 4 // this means that the car can turn on the A/C if necessary
        uint8_t load_shedding_level3_possible : 1; // bit 5
        uint8_t load_shedding_level2_possible : 1; // bit 6
        uint8_t load_shedding_level1_possible : 1; // bit 7
    } data;
    uint8_t asByte;
};

// Read left to right in documentation
struct VanAirConditioner1Struct {
    VanAirCon1Byte0Struct Status;
    uint8_t Byte1;
    uint8_t Byte2;
    uint8_t Byte3;
    uint8_t PowerConsumption;//we must multiply by 25 to get the value in watts
};

union VanAirConditioner1Packet {
    VanAirConditioner1Struct data;
    uint8_t VanAirConditioner1Packet[sizeof(VanAirConditioner1Struct)];
};

#endif
