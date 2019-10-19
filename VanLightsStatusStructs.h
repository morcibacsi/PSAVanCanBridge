// VanLightsStatusStructs.h
#pragma once

#ifndef _VanLightsStatusStructs_h
    #define _VanLightsStatusStructs_h

// VANID: 4FC
const uint16_t VAN_ID_LIGHTS_STATUS = 0x4FC;

// Read right to left in documentation
typedef struct {
    uint8_t unknown0          : 1; // bit 0
    uint8_t unknown1          : 1; // bit 1
    uint8_t hazard_warning_on : 1; // bit 2
    uint8_t unknown3          : 1; // bit 3
    uint8_t unknown4          : 1; // bit 4
    uint8_t unknown5          : 1; // bit 5
    uint8_t unknown6          : 1; // bit 6
    uint8_t unknown7          : 1; // bit 7
} VanLightsStatusByte0Struct;

// Read right to left in documentation
typedef struct {
    uint8_t unknown0        : 1; // bit 0
    uint8_t unknown1        : 1; // bit 1
    uint8_t left_indicator  : 1; // bit 2
    uint8_t right_indicator : 1; // bit 3
    uint8_t rear_fog        : 1; // bit 4
    uint8_t front_fog       : 1; // bit 5
    uint8_t high_beam       : 1; // bit 6
    uint8_t dipped_beam     : 1; // bit 7
} VanLightsStatusByte5Struct;

// Read left to right in documentation
typedef struct VanLightsStatusStructs {
    VanLightsStatusByte0Struct Field0;
    uint8_t Field1;
    uint8_t Field2;
    uint8_t Field3;
    uint8_t Field4;
    VanLightsStatusByte5Struct LightsStatus;
    uint8_t Field6;
    uint8_t FuelLevel; // in percentage
    uint8_t Field8;
    uint8_t Field9;
    uint8_t Field10;
};

typedef union VanLightStatusPacket {
    VanLightsStatusStructs data;
    uint8_t VanLightStatusPacket[sizeof(VanLightsStatusStructs)];
};

#endif
