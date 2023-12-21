// CanDash3Structs.h
#pragma once

#ifndef _CanDash3Structs_h
    #define _CanDash3Structs_h

#include <stdint.h>

// CANID: 168
const uint16_t CAN_ID_DASH3 = 0x168;

// Read right to left in documentation
union CanDash3Byte1Struct {
    struct {
        uint8_t                       : 1; // bit 0
        uint8_t auto_gearbox_alert    : 1; // bit 1
        uint8_t break_fluid_alert     : 1; // bit 2
        uint8_t oil_pressure_alert    : 1; // bit 3
        uint8_t oil_level_alert       : 1; // bit 4
        uint8_t coolant_level_alert   : 1; // bit 5
        uint8_t oil_temp_max          : 1; // bit 6
        uint8_t coolant_temp_max      : 1; // bit 7
    } data;
    uint8_t asByte;
};

union CanDash3Byte2Struct {
    struct {
        uint8_t                       : 1; // bit 0
        uint8_t                       : 1; // bit 1
        uint8_t                       : 1; // bit 2
        uint8_t                       : 1; // bit 3
        uint8_t fap_clogged           : 1; // bit 4
        uint8_t diesel_additive_alert : 1; // bit 5
        uint8_t flat_tyre_alert       : 1; // bit 6
        uint8_t tyre_pressure_alert   : 1; // bit 7
    } data;
    uint8_t asByte;
};

union CanDash3Byte4Struct {
    struct {
        uint8_t water_in_diesel          : 1; // bit 0
        uint8_t mil                      : 1; // bit 1
        uint8_t brake_pad_fault          : 1; // bit 2
        uint8_t gearbox_fault            : 1; // bit 3
        uint8_t esp_fault                : 1; // bit 4
        uint8_t abs_fault                : 1; // bit 5
        uint8_t serious_suspension_fault : 1; // bit 6
        uint8_t serious_ref_ehb_fault    : 1; // bit 7
    } data;
    uint8_t asByte;
};

union CanDash3Byte5Struct {
    struct {
        uint8_t lamp_bulb_fault       : 1; // bit 0
        uint8_t generator_fault       : 1; // bit 1
        uint8_t battery_charge_fault  : 1; // bit 2
        uint8_t diesel_additive_fault : 1; // bit 3
        uint8_t antipollution_fault   : 1; // bit 4
        uint8_t driver_airbag         : 1; // bit 5
        uint8_t auto_lighting_fault   : 1; // bit 6
        uint8_t wiper_fault           : 1; // bit 7
    } data;
    uint8_t asByte;
};


// Read left to right in documentation
struct CanDash3Struct {
    CanDash3Byte1Struct Field1;
    CanDash3Byte2Struct Field2;
    uint8_t Field3;
    CanDash3Byte4Struct Field4;
    CanDash3Byte5Struct Field5;
    uint8_t Field6;
    uint8_t Field7;
    uint8_t Field8;
};

union CanDash3Packet {
    CanDash3Struct data;
    uint8_t CanDash3Packet[sizeof(CanDash3Struct)];
};

#endif
