// VanInstrumentClusterV2Structs.h
#pragma once

#ifndef _VAN_4FC_h
    #define _VAN_4FC_h

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
    uint8_t esp_in_progress              : 1; // bit 1
    uint8_t pre_heating_status           : 1; // bit 2
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
union VanInstrumentClusterV2Byte3Struct {
    struct {
        uint8_t maintenance_km1   : 7; // bit 6
        uint8_t maintenance_due   : 1; // bit 7
    } data;
    uint8_t asByte;
};

// Read right to left in documentation
typedef struct {
    uint8_t bva_bvmp_selection      : 1; // bit 0
    uint8_t gearbox_selection_mode  : 3; // bit 1-3
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

typedef struct {
    uint8_t seatbelt_not_fastened : 1; // bit 0
    uint8_t appui_push_button     : 1; // bit 1
    uint8_t odb_cmb               : 1; // bit 2
    uint8_t body_position_level   : 3; // bit 3-5
    uint8_t body_position_status  : 2; // bit 6-7
} VanInstrumentClusterV2Byte10Struct;

union VAN_4FC_Byte12Struct{
    struct {
        uint8_t function_alert              : 1; // bit 0
        uint8_t speed_unit                  : 1; // bit 1
        uint8_t activate_function           : 1; // bit 2
        uint8_t status_of_selected_function : 3; // bit 3-5
        uint8_t selected_function           : 2; // bit 6-7
    } data;
    uint8_t asByte;
};


/*
union VanInstrumentClusterV2Byte12Struct {
    struct
    {
        uint8_t unknown0 : 1;
        uint8_t unknown1     : 1;
        uint8_t unknown2     : 1;
        uint8_t cc_on        : 1;
        uint8_t cc_speed_blinking : 1;
        uint8_t unknown5    : 1;
        uint8_t cc_on       : 1;
        uint8_t limiter_on  : 1;
    }data;
    uint8_t asByte;
};
*/
// Read left to right in documentation
struct VanInstrumentClusterV2Structs {
    VanInstrumentClusterV2Byte1Struct Field1;
    VanInstrumentClusterV2Byte2Struct Field2;
    VanInstrumentClusterV2Byte3Struct MaintenanceKmByte1;
    uint8_t MaintenanceKmByte2;
    VanInstrumentClusterV2Byte5Struct AutomaticGearbox;
    VanInstrumentClusterV2Byte6Struct LightsStatus;
    uint8_t OilTemperature;
    uint8_t FuelLevel; // in percentage
    uint8_t OilLevel;
    VanInstrumentClusterV2Byte10Struct Field10;
    uint8_t LPGFuelLevel;
    VAN_4FC_Byte12Struct CruiseControlStatus;
    uint8_t CruiseControlSpeedByte1;
    uint8_t CruiseControlSpeedByte2;
};

#endif
