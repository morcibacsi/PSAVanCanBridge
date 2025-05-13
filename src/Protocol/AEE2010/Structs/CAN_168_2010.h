// CanDash3Structs.h
#pragma once

#ifndef _2010_168_h
    #define _2010_168_h

#include <stdint.h>

// CANID: 168
const uint16_t CAN_ID_DASH3_2010 = 0x168;

// Read right to left in documentation
union CanDash3Byte1_2010_Struct {
    struct {
        uint8_t dsg_fault             : 1; // bit 0
        uint8_t engine_cold           : 1; // bit 1
        uint8_t break_fluid_alert     : 1; // bit 2
        uint8_t oil_pressure_alert    : 1; // bit 3
        uint8_t oil_level_alert       : 1; // bit 4
        uint8_t coolant_level_alert   : 1; // bit 5
        uint8_t number_of_gears       : 1; // bit 6
        uint8_t coolant_temp_max      : 1; // bit 7
    } data;
    uint8_t asByte;
};

union CanDash3Byte2_2010_Struct {
    struct {
        uint8_t command_plus_start    : 1; // bit 0
        uint8_t max_rpm_2             : 1; // bit 1
        uint8_t max_rpm_1             : 1; // bit 2
        uint8_t auto_wiping_active    : 1; // bit 3
        uint8_t fap_clogged           : 1; // bit 4
        uint8_t antipollution_fault   : 1; // bit 5
        uint8_t tyre_punctured        : 1; // bit 6
        uint8_t tyre_pressure_low     : 1; // bit 7
    } data;
    uint8_t asByte;
};

union CanDash3Byte3_2010_Struct {
    struct {
        uint8_t generator_fault          : 1; // bit 0
        uint8_t battery_charge_fault     : 1; // bit 1
        uint8_t serious_suspension_fault : 1; // bit 2
        uint8_t serious_ref_ehb_fault    : 1; // bit 3
        uint8_t row2_rr_seatbelt_blink   : 1; // bit 4
        uint8_t row2_rr_seatbelt_forgot  : 1; // bit 5
        uint8_t row2_rl_seatbelt_blink   : 1; // bit 6
        uint8_t row2_rl_seatbelt_forgot  : 1; // bit 7
    } data;
    uint8_t asByte;
};

union CanDash3Byte4_2010_Struct {
    struct {
        uint8_t mil_update_bit           : 1; // bit 0
        uint8_t mil                      : 1; // bit 1
        uint8_t brake_pad_fault          : 1; // bit 2
        uint8_t gearbox_fault            : 1; // bit 3
        uint8_t esp_fault                : 1; // bit 4
        uint8_t abs_fault                : 1; // bit 5
        uint8_t fse_tightening_fault     : 1; // bit 6
        uint8_t fse_system_fault         : 1; // bit 7
    } data;
    uint8_t asByte;
};

union CanDash3Byte5_2010_Struct {
    struct {
        uint8_t stt_lamp_status     : 2;  // bit 0-1
        uint8_t power_steering_fault: 1;  // bit 2
        uint8_t caar_lamp_status    : 2;  // bit 3-4
        uint8_t passive_safety_fault: 1;  // bit 5
        uint8_t curve_code_fault    : 1;  // bit 6
        uint8_t water_in_diesel     : 1;  // bit 7
    } data;
    uint8_t asByte;
};

union CanDash3Byte7_2010_Struct {
    struct {
        uint8_t brake_assist_lamp : 1;  // bit 0
        uint8_t gearbox_position  : 4;  // bit 1-4
        uint8_t ack_emf_presence  : 1;  // bit 5
        uint8_t authorize_ambience: 1;  // bit 6
        uint8_t authorize_vth     : 1;  // bit 7
    } data;
    uint8_t asByte;
};

union CanDash3Byte8_2010_Struct {
    struct {
        uint8_t hdc_warning_lamp_status: 2;  // bit 0-1
        uint8_t afil_lamp_status       : 2;  // bit 2-3
        uint8_t vth_right_warning      : 1;  // bit 4
        uint8_t vth_left_warning       : 1;  // bit 5
        uint8_t farc_lamp_status       : 2;  // bit 6-7
    } data;
    uint8_t asByte;
};


// Read left to right in documentation
struct CanDash3_2010Struct {
    CanDash3Byte1_2010_Struct Field1;
    CanDash3Byte2_2010_Struct Field2;
    CanDash3Byte3_2010_Struct Field3;
    CanDash3Byte4_2010_Struct Field4;
    CanDash3Byte5_2010_Struct Field5;
    uint8_t Field6;
    CanDash3Byte7_2010_Struct Field7;
    CanDash3Byte8_2010_Struct Field8;
};

union CanDash3_2010Packet {
    CanDash3_2010Struct data;
    uint8_t CanDash3Packet[sizeof(CanDash3_2010Struct)];
};

#endif
