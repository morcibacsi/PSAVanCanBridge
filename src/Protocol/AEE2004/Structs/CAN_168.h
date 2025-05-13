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
        uint8_t dsg_fault             : 1; // bit 0
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
        uint8_t max_rpm_2             : 1; // bit 0
        uint8_t minc_blinking         : 1; // bit 1
        uint8_t max_rpm_1             : 1; // bit 2
        uint8_t auto_wiping_active    : 1; // bit 3
        uint8_t fap_clogged           : 1; // bit 4
        uint8_t diesel_additive_alert : 1; // bit 5
        uint8_t tyre_punctured        : 1; // bit 6
        uint8_t tyre_pressure_low     : 1; // bit 7
    } data;
    uint8_t asByte;
};

union CanDash3Byte3Struct {
    struct {
        uint8_t foot_on_clutch                      : 2; // bit 0-1
        uint8_t row2_rr_seatbelt_forgotten_blinking : 1; // bit 2
        uint8_t row2_rr_seatbelt_forgotten          : 1; // bit 3
        uint8_t row2_rc_seatbelt_forgotten_blinking : 1; // bit 4
        uint8_t row2_rc_seatbelt_forgotten          : 1; // bit 5
        uint8_t row2_rl_seatbelt_forgotten_blinking : 1; // bit 6
        uint8_t row2_rl_seatbelt_forgotten          : 1; // bit 7
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
        uint8_t lamp_bulb_fault       : 1; // bit 0 or last bit of gear_position_in_driving ???
        uint8_t generator_fault       : 1; // bit 1
        uint8_t battery_charge_fault  : 1; // bit 2
        uint8_t diesel_additive_fault : 1; // bit 3
        uint8_t antipollution_fault   : 1; // bit 4
        uint8_t driver_airbag         : 1; // bit 5
        uint8_t scr_indicator         : 2; // bit 6-7
        //uint8_t auto_lighting_fault   : 1; // bit 6
        //uint8_t wiper_fault           : 1; // bit 7
    } data;
    uint8_t asByte;
};

union CanDash3Byte6Struct {
    struct {
        uint8_t engine_fault             : 1; // bit 0
        uint8_t                          : 1; // bit 1
        uint8_t curve_code_fault         : 1; // bit 2
        uint8_t caar_lamp_status         : 2; // bit 3-4
        uint8_t gear_position_in_driving : 3; // bit 5-7
    } data;
    uint8_t asByte;
};

union CanDash3Byte7Struct {
    struct {
        uint8_t stt_lamp_status          : 2; // bit 0-1
        uint8_t engine_fault_blinking    : 1; // bit 2
        uint8_t fse_tightening_fault     : 1; // bit 3
        uint8_t fse_system_fault         : 1; // bit 4
        uint8_t power_steering_fault     : 1; // bit 4
        uint8_t                          : 2; // bit 4
    } data;
    uint8_t asByte;
};

union CanDash3Byte8Struct {
    struct {
        uint8_t stt_lamp_status          : 2; // bit 0-1
        uint8_t zev_hybrid               : 2; // bit 2-3
        uint8_t                          : 2; // bit 4-5
        uint8_t ready_status             : 1; // bit 6
        uint8_t pare_fault               : 1; // bit 7
    } data;
    uint8_t asByte;
};


// Read left to right in documentation
struct CanDash3Struct {
    CanDash3Byte1Struct Field1;
    CanDash3Byte2Struct Field2;
    CanDash3Byte3Struct Field3;
    CanDash3Byte4Struct Field4;
    CanDash3Byte5Struct Field5;
    CanDash3Byte6Struct Field6;
    CanDash3Byte7Struct Field7;
    CanDash3Byte8Struct Field8;
};

union CanDash3Packet {
    CanDash3Struct data;
    uint8_t CanDash3Packet[sizeof(CanDash3Struct)];
};

#endif
