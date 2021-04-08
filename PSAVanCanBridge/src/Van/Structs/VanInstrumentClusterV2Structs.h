// VanInstrumentClusterV2Structs.h
#pragma once

#ifndef _VanInstrumentClusterV2Structs_h
    #define _VanInstrumentClusterV2Structs_h

// VANID: 4FC
const uint16_t VAN_ID_INSTRUMENT_CLUSTER_V2 = 0x4FC;

const uint8_t VAN_GEAR_V2_P = 0;
const uint8_t VAN_GEAR_V2_R = 1;
const uint8_t VAN_GEAR_V2_N = 2;
const uint8_t VAN_GEAR_V2_D = 3;
const uint8_t VAN_GEAR_V2_4 = 4;
const uint8_t VAN_GEAR_V2_3 = 5;
const uint8_t VAN_GEAR_V2_2 = 6;
const uint8_t VAN_GEAR_V2_1 = 7;

const uint8_t VAN_GEAR_MODE_V2_NORMAL              = 0;
const uint8_t VAN_GEAR_MODE_V2_AUTO_ECO            = 1;
const uint8_t VAN_GEAR_MODE_V2_SPORT               = 2;
const uint8_t VAN_GEAR_MODE_V2_SEQUENTIAL_ECO      = 3;
const uint8_t VAN_GEAR_MODE_V2_SNOW_AUTO           = 4;
const uint8_t VAN_GEAR_MODE_V2_SNOW_AUTO_ECO       = 5;
const uint8_t VAN_GEAR_MODE_V2_SNOW_SEQUENTIAL     = 6;
const uint8_t VAN_GEAR_MODE_V2_SNOW_SEQUENTIAL_ECO = 7;

const uint8_t VAN_GEAR_V2_SELECTION_BVA = 0;
const uint8_t VAN_GEAR_V2_SELECTION_BVM = 1;

const uint8_t VAN_CC_OFF           = 0x41;
const uint8_t VAN_CC_ON            = 0x49;
const uint8_t VAN_CC_SPEED_BLINK   = 0x59;
const uint8_t VAN_CC_LIMITER       = 0x81;
const uint8_t VAN_CC_LIMITER_BLINK = 0x89;

// Read right to left in documentation
typedef struct {
    uint8_t memo_status                  : 1; // bit 0
    uint8_t asr_abs_enabled              : 1; // bit 1
    uint8_t hazard_warning_on            : 1; // bit 2
    uint8_t ac_recycling_status          : 1; // bit 3
    uint8_t rear_window_heating_status   : 1; // bit 4
    uint8_t hazard_warning_button_status : 1; // bit 5
    uint8_t tempomat_enabled             : 1; // bit 6
    uint8_t cluster_enabled              : 1; // bit 7
} VanInstrumentClusterV2Byte1Struct;

// Read right to left in documentation
typedef struct {
    uint8_t door_open_status    : 1; // bit 0
    uint8_t digital_out4_status : 1; // bit 1
    uint8_t digital_out3_status : 1; // bit 2
    uint8_t digital_out2_status : 1; // bit 3
    uint8_t digital_out1_status : 1; // bit 4
    uint8_t alarm_button_status : 1; // bit 5
    uint8_t suspension_status   : 1; // bit 6
    uint8_t fault_lamp_status   : 1; // bit 7
} VanInstrumentClusterV2Byte2Struct;

// Read right to left in documentation
typedef struct {
    uint8_t bva_bvmp_selection      : 1; // bit 0
    uint8_t gearbox_selection_mode  : 1; // bit 1-3
    uint8_t gear_position           : 4; // bit 4-7
} VanInstrumentClusterV2Byte5Struct;

// Read right to left in documentation
typedef struct {
    uint8_t compare_lever_and_engaged    : 1; // bit 0
    uint8_t auto_gearbox_display_enabled : 1; // bit 1
    uint8_t left_indicator               : 1; // bit 2
    uint8_t right_indicator              : 1; // bit 3
    uint8_t rear_fog                     : 1; // bit 4
    uint8_t front_fog                    : 1; // bit 5
    uint8_t high_beam                    : 1; // bit 6
    uint8_t dipped_beam                  : 1; // bit 7
} VanInstrumentClusterV2Byte6Struct;

// Read left to right in documentation
struct VanInstrumentClusterV2Structs {
    VanInstrumentClusterV2Byte1Struct Field1;
    VanInstrumentClusterV2Byte2Struct Field2;
    uint8_t ServiceIntervalByte1;
    uint8_t ServiceIntervalByte2;
    VanInstrumentClusterV2Byte5Struct AutomaticGearbox;
    VanInstrumentClusterV2Byte6Struct LightsStatus;
    uint8_t OilTemperature;
    uint8_t FuelLevel; // in percentage
    uint8_t OilLevel;
    uint8_t Field10;
    uint8_t LPGFuelLevel;
    uint8_t CruiseControlStatus;
    uint8_t CruiseControlSpeed;
    uint8_t Field14;
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
