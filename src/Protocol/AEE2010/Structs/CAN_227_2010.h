// CAN_228_2010.h
#pragma once

#ifndef _CAN_227_2010_h
    #define _CAN_227_2010_h

#include <stdint.h>

const uint16_t CAN_227_2010_INTERVAL = 500;

// CANID: 227
const uint16_t CAN_ID_227_2010 = 0x227;

constexpr uint8_t SETTING_STATUS_NO_ADJUSTMENT = 0;
constexpr uint8_t SETTING_STATUS_ADJUSTMENT = 1;

// Read right to left in documentation
union CAN_227_2010_Byte1Struct {
    struct {
        uint8_t amvar_sport_led : 2; // bit 0-1
        uint8_t child_lock_led  : 2; // bit 2-3
        uint8_t esp_led         : 2; // bit 4-5
        uint8_t aas_led         : 2; // bit 6-7
    } data;
    uint8_t asByte;
};

union CAN_227_2010_Byte2Struct {
    struct {
        uint8_t ac_on_led                    : 2; // bit 0-1
        uint8_t rear_window_heating_led      : 2; // bit 2-3
        uint8_t stl_led                      : 2; // bit 4-5
        uint8_t secondary_electric_brake_led : 2; // bit 6-7
    } data;
    uint8_t asByte;
};

union CAN_227_2010_Byte3Struct {
    struct {
        uint8_t blind_spot_monitoring_led : 2; // bit 0-1
        uint8_t ac_airflow_type           : 2; // bit 2-3
        uint8_t fuel_info                 : 2; // bit 4-5
        uint8_t stop_start_led            : 2; // bit 6-7
    } data;
    uint8_t asByte;
};

union CAN_227_2010_Byte4Struct {
    struct {
        uint8_t dynamic_led          : 2; // bit 0-1
        uint8_t stt_led              : 2; // bit 2-3
        uint8_t cafr_led             : 1; // bit 4
        uint8_t artiv_led            : 2; // bit 5-6
        uint8_t ac_airflow_recycling : 1; // bit 7
    } data;
    uint8_t asByte;
};

union CAN_227_2010_Byte5Struct {
    struct {
        uint8_t welcome_light_led : 1; // bit 0
        uint8_t auto_wiping       : 2; // bit 1-2
        uint8_t hadc_led          : 1; // bit 3
        uint8_t lka_led           : 2; // bit 4-5
        uint8_t achv_led          : 2; // bit 6-7
    } data;
    uint8_t asByte;
};

union CAN_227_2010_Byte6Struct {
    struct {
        uint8_t energy_recover_led : 1; // bit 0
        uint8_t avas_led           : 1; // bit 1
        uint8_t eco_dyn_mode       : 2; // bit 2-3
        uint8_t avn_led            : 2; // bit 4-5
        uint8_t lpa_led            : 2; // bit 6-7
    } data;
    uint8_t asByte;
};

union CAN_227_2010_Byte7Struct {
    struct {
        uint8_t sound_request : 7; // bit 0-6
        uint8_t unused        : 1; // bit 7
    } data;
    uint8_t asByte;
};

// Read left to right in documentation
struct CAN_227_2010_Struct {
    CAN_227_2010_Byte1Struct Field1;
    CAN_227_2010_Byte2Struct Field2;
    CAN_227_2010_Byte3Struct Field3;
    CAN_227_2010_Byte4Struct Field4;
    CAN_227_2010_Byte5Struct Field5;
    CAN_227_2010_Byte6Struct Field6;
    CAN_227_2010_Byte7Struct Field7;
};
#endif
