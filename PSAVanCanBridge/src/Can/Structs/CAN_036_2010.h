#pragma once

#ifndef _CanIgnitionStructs_2010_h
    #define _CanIgnitionStructs_2010_h

#include <stdint.h>

// CANID: 036
const uint16_t CAN_ID_BSI_COMMANDS_2010 = 0x036;

const int CAN_2010_IGNITION_MODE_STANDBY           = 0x00;
const int CAN_2010_IGNITION_MODE_NORMAL            = 0x01;
const int CAN_2010_IGNITION_MODE_STANDBY_SOON      = 0x02;
const int CAN_2010_IGNITION_MODE_WAKE_UP           = 0x03;
const int CAN_2010_IGNITION_MODE_COM_OFF           = 0x04;

const uint8_t DRIVING_DIRECTION_UNDEFINED     = 0x00;
const uint8_t DRIVING_DIRECTION_FORWARD       = 0x01;
const uint8_t DRIVING_DIRECTION_REVERSE       = 0x02;
const uint8_t DRIVING_DIRECTION_INVALID       = 0x03;

// Read right to left in documentation
union CAN_036_2010_Byte1Struct{
    struct {
        uint8_t memory_slot          : 4; // bit 0-4
        uint8_t driver_memo_sequence : 1; // bit 5-6
        uint8_t driver_memo_recall   : 1; // bit 5-6
        uint8_t driving_direction    : 2; // bit 6-7
    } data;
    uint8_t asByte;
};

// Read right to left in documentation
union CAN_036_2010_Byte3Struct{
    struct {
        uint8_t ambience_level        : 7; // bit 5-6
        uint8_t economy_mode_active   : 1; // bit 7
    } data;
    uint8_t asByte;
};

union CAN_036_2010_Byte4Struct {
    struct {
        uint8_t dashboard_brightness       : 4; // bit 0-3
        uint8_t black_panel_status         : 1; // bit 4
        uint8_t night_mode                 : 1; // bit 5
        uint8_t rheostat_type              : 1; // bit 6
        uint8_t resync_of_reference_level  : 1; // bit 7
    } data;
    uint8_t asByte;
};

union CAN_036_2010_Byte5Struct{
    struct {
        uint8_t ignition_mode               : 3; // bit 0-2
        uint8_t prevent_fault_log           : 1; // bit 3
        uint8_t internal_diag_tool_enabled  : 1; // bit 4
        uint8_t network_supervision_enabled : 1; // bit 5
        uint8_t global_fault_clearance      : 1; // bit 6
        uint8_t dynamic_function_active     : 1; // bit 7
    } data;
    uint8_t asByte;
};

// Read left to right in documentation
struct CAN_036_2010_Struct {
    CAN_036_2010_Byte1Struct Byte1;
    uint8_t Unused;
    CAN_036_2010_Byte3Struct Byte3;
    CAN_036_2010_Byte4Struct Brightness;
    CAN_036_2010_Byte5Struct Ignition;
    uint8_t HybridMode;
    uint8_t Reserved7;
    uint8_t Unused8;
};

union CAN_036_2010_Packet {
    CAN_036_2010_Struct data;
    uint8_t CAN_036_2010_Packet[sizeof(CAN_036_2010_Struct)];
};
#pragma endregion

#endif
