#pragma once

#include <inttypes.h>

//These are in the same order as in the AEE2004/2010 0x168 so don't change the order
union CarIndicatorLightsStruct {
    struct {
        //1
        uint8_t dsg_fault             : 1; // bit 0
        uint8_t engine_cold           : 1; // bit 1
        uint8_t break_fluid_alert     : 1; // bit 2
        uint8_t oil_pressure_alert    : 1; // bit 3
        uint8_t oil_level_alert       : 1; // bit 4
        uint8_t coolant_level_alert   : 1; // bit 5
        uint8_t number_of_gears       : 1; // bit 6
        uint8_t coolant_temp_max      : 1; // bit 7
        //2
        uint8_t command_plus_start    : 1; // bit 0
        uint8_t max_rpm_2             : 1; // bit 1
        uint8_t max_rpm_1             : 1; // bit 2
        uint8_t auto_wiping_active    : 1; // bit 3
        uint8_t fap_clogged           : 1; // bit 4
        uint8_t antipollution_fault   : 1; // bit 5
        uint8_t tyre_punctured        : 1; // bit 6
        uint8_t tyre_pressure_low     : 1; // bit 7
        //3
        uint8_t generator_fault          : 1; // bit 0
        uint8_t battery_charge_fault     : 1; // bit 1
        uint8_t serious_suspension_fault : 1; // bit 2
        uint8_t serious_ref_ehb_fault    : 1; // bit 3
        uint8_t row2_rr_seatbelt_blink   : 1; // bit 4
        uint8_t row2_rr_seatbelt_forgot  : 1; // bit 5
        uint8_t row2_rl_seatbelt_blink   : 1; // bit 6
        uint8_t row2_rl_seatbelt_forgot  : 1; // bit 7
        //4
        uint8_t mil_update_bit           : 1; // bit 0
        uint8_t mil                      : 1; // bit 1
        uint8_t brake_pad_fault          : 1; // bit 2
        uint8_t gearbox_fault            : 1; // bit 3
        uint8_t esp_fault                : 1; // bit 4
        uint8_t abs_fault                : 1; // bit 5
        uint8_t fse_tightening_fault     : 1; // bit 6
        uint8_t fse_system_fault         : 1; // bit 7
        //5
        uint8_t stt_lamp_status     : 2;  // bit 0-1
        uint8_t power_steering_fault: 1;  // bit 2
        uint8_t caar_lamp_status    : 2;  // bit 3-4
        uint8_t passive_safety_fault: 1;  // bit 5
        uint8_t curve_code_fault    : 1;  // bit 6
        uint8_t water_in_diesel     : 1;  // bit 7
        //7
        uint8_t brake_assist_lamp : 1;  // bit 0
        uint8_t gearbox_position  : 4;  // bit 1-4
        uint8_t ack_emf_presence  : 1;  // bit 5
        uint8_t authorize_ambience: 1;  // bit 6
        uint8_t authorize_vth     : 1;  // bit 7
        //8
        uint8_t hdc_warning_lamp_status: 2;  // bit 0-1
        uint8_t afil_lamp_status       : 2;  // bit 2-3
        uint8_t vth_right_warning      : 1;  // bit 4
        uint8_t vth_left_warning       : 1;  // bit 5
        uint8_t farc_lamp_status       : 2;  // bit 6-7
    } data;
    uint64_t asUint64;
};
