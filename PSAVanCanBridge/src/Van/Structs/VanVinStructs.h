// VanVinStructs.h
#pragma once

#ifndef _VanVinStructs_h
    #define _VanVinStructs_h

// VANID: E24
const uint16_t VAN_ID_VIN = 0xE24;

// Read left to right in documentation
struct VanVinStruct {
    uint8_t Vin1;
    uint8_t Vin2;
    uint8_t Vin3;
    uint8_t Vin4;
    uint8_t Vin5;
    uint8_t Vin6;
    uint8_t Vin7;
    uint8_t Vin8;
    uint8_t Vin9;
    uint8_t Vin10;
    uint8_t Vin11;
    uint8_t Vin12;
    uint8_t Vin13;
    uint8_t Vin14;
    uint8_t Vin15;
    uint8_t Vin16;
    uint8_t Vin17;
};

union VanVinPacket {
    VanVinStruct data;
    uint8_t VanVinPacket[sizeof(VanVinStruct)];
};

#endif
