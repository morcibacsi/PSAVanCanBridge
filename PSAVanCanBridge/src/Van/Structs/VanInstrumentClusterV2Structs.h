// VanInstrumentClusterV2Structs.h
#pragma once

#ifndef _VanInstrumentClusterV2Structs_h
    #define _VanInstrumentClusterV2Structs_h

// VANID: 4FC
const uint16_t VAN_ID_INSTRUMENT_CLUSTER_V2 = 0x4FC;

const uint8_t VAN_GEAR_V2_P = 0x00;
const uint8_t VAN_GEAR_V2_R = 0x10;
const uint8_t VAN_GEAR_V2_N = 0x20;
const uint8_t VAN_GEAR_V2_D = 0x30;
const uint8_t VAN_GEAR_V2_4 = 0x40;
const uint8_t VAN_GEAR_V2_3 = 0x50;
const uint8_t VAN_GEAR_V2_2 = 0x60;
const uint8_t VAN_GEAR_V2_1 = 0x70;

const uint8_t VAN_CC_OFF           = 0x41;
const uint8_t VAN_CC_ON            = 0x49;
const uint8_t VAN_CC_SPEED_BLINK   = 0x59;
const uint8_t VAN_CC_LIMITER       = 0x81;
const uint8_t VAN_CC_LIMITER_BLINK = 0x89;

// Read right to left in documentation
typedef struct {
    uint8_t unknown0          : 1; // bit 0
    uint8_t unknown1          : 1; // bit 1
    uint8_t hazard_warning_on : 1; // bit 2
    uint8_t unknown3          : 1; // bit 3
    uint8_t unknown4          : 1; // bit 4
    uint8_t unknown5          : 1; // bit 5
    uint8_t tempomat_enabled  : 1; // bit 6
    uint8_t cluster_enabled   : 1; // bit 7
} VanInstrumentClusterV2Byte0Struct;

// Read right to left in documentation
typedef struct {
    uint8_t unknown0        : 1; // bit 0
    uint8_t unknown1        : 1; // bit 1
    uint8_t sport_mode      : 1; // bit 2
    uint8_t snow_mode       : 1; // bit 3
    uint8_t gear            : 3; // bit 4
    uint8_t blinking        : 1; // bit 5
} VanInstrumentClusterV2Byte4Struct;

// Read right to left in documentation
typedef struct {
    uint8_t unknown0                     : 1; // bit 0
    uint8_t auto_gearbox_display_enabled : 1; // bit 1
    uint8_t left_indicator               : 1; // bit 2
    uint8_t right_indicator              : 1; // bit 3
    uint8_t rear_fog                     : 1; // bit 4
    uint8_t front_fog                    : 1; // bit 5
    uint8_t high_beam                    : 1; // bit 6
    uint8_t dipped_beam                  : 1; // bit 7
} VanInstrumentClusterV2Byte5Struct;

// Read left to right in documentation
struct VanInstrumentClusterV2Structs {
    VanInstrumentClusterV2Byte0Struct Field0;
    uint8_t Field1;
    uint8_t ServiceIntervalByte1;
    uint8_t ServiceIntervalByte2;
    uint8_t AutomaticGearbox;
    VanInstrumentClusterV2Byte5Struct LightsStatus;
    uint8_t OilTemperature;
    uint8_t FuelLevel; // in percentage
    uint8_t OilLevel;
    uint8_t Field9;
    uint8_t LPGFuelLevel;
    uint8_t CruiseControlStatus;
    uint8_t CruiseControlSpeed;
    uint8_t Field13;
};

union VanInstrumentClusterPacketV2 {
    VanInstrumentClusterV2Structs data;
    uint8_t VanInstrumentClusterV2Packet[sizeof(VanInstrumentClusterV2Structs)];
};

int8_t static GetOilTemperatureFromVANByteV2(uint8_t vanByte)
{
    return vanByte - 40;
}

uint8_t static GetVANByteFromOilTemperatureV2(int8_t temperature)
{
    return temperature + 40;
}

#endif
