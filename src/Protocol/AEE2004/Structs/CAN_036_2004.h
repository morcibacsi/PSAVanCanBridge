#pragma once

#ifndef _CanIgnitionStructs_h
    #define _CanIgnitionStructs_h

#include <stdint.h>

// CANID: 036
const uint16_t CAN_ID_BSI_COMMANDS = 0x036;

const int CAN_IGNITION_MODE_STANDBY           = 0x00;
const int CAN_IGNITION_MODE_NORMAL            = 0x01;
const int CAN_IGNITION_MODE_STANDBY_SOON      = 0x02;
const int CAN_IGNITION_MODE_WAKE_UP           = 0x03;
const int CAN_IGNITION_MODE_COM_OFF           = 0x04;

// Read right to left in documentation
typedef struct {
    uint8_t memory_slot                   : 4; // bit 0-3
    uint8_t memory_ordered                : 1; // bit 4
    uint8_t passenger_memory_recall_order : 1; // bit 5
    uint8_t ihm_profile                   : 2; // bit 6-7
} CanIgnitionMemoStruct;

// Read right to left in documentation
union CanIgnitionByte3Struct{
    struct {
        uint8_t load_shedding_level   : 5; // bit 0-4
        uint8_t unused56              : 2; // bit 5-6
        uint8_t economy_mode_active   : 1; // bit 7
    } data;
    uint8_t asByte;
};

union CanIgnitionByte4Struct {
    struct {
        uint8_t dashboard_brightness       : 4; // bit 0-3
        uint8_t black_panel_status         : 1; // bit 4
        uint8_t night_mode                 : 1; // bit 5
        uint8_t unused6                    : 1; // bit 6
        uint8_t resync_of_reference_level  : 1; // bit 7
    } data;
    uint8_t asByte;
};

union CanIgnitionByte5Struct{
    struct {
        uint8_t ignition_mode               : 3; // bit 0-2
        uint8_t prevent_fault_log           : 1; // bit 3
        uint8_t unused4                     : 1; // bit 4
        uint8_t network_supervision_enabled : 1; // bit 5
        uint8_t global_fault_clearance      : 1; // bit 6
        uint8_t unused7                     : 1; // bit 7
    } data;
    uint8_t asByte;
};

union CanIgnitionByte7Struct{
    struct {
        uint8_t unknown0               : 1; // bit 0
        uint8_t unknown1               : 1; // bit 1
        uint8_t unknown2               : 1; // bit 2
        uint8_t activate_rear_camera   : 1; // bit 3
        uint8_t unknown4               : 1; // bit 4
        uint8_t unknown5               : 1; // bit 5
        uint8_t unknown6               : 1; // bit 6
        uint8_t unknown7               : 1; // bit 7
    } data;
    uint8_t asByte;
};

// Read left to right in documentation
struct CanIgnitionStruct {
    CanIgnitionMemoStruct Memory1;
    CanIgnitionMemoStruct Memory2;
    CanIgnitionByte3Struct LoadShedding;
    CanIgnitionByte4Struct Brightness;
    CanIgnitionByte5Struct Ignition;
    uint8_t Reserved6;
    CanIgnitionByte7Struct RearCamera;
    uint8_t Checksum;
};

union CanIgnitionPacket {
    CanIgnitionStruct data;
    uint8_t CanIgnitionPacket[sizeof(CanIgnitionStruct)];
};
#pragma endregion

#endif
