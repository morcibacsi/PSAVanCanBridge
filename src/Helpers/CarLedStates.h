#pragma once

#include <cstdint>

//These are in the same order as in the AEE2004/2010 so don't change the order
union CarLedStatusByte1Struct {
    struct {
        uint8_t amvar_sport_led : 2; // bit 0-1
        uint8_t child_lock_led  : 2; // bit 2-3
        uint8_t esp_led         : 2; // bit 4-5
        uint8_t aas_led         : 2; // bit 6-7
    } data;
    uint8_t asByte;
};

union CarLedStatusByte2Struct {
    struct {
        uint8_t ac_on_led                    : 2; // bit 0-1
        uint8_t rear_window_heating_led      : 2; // bit 2-3
        uint8_t stl_led                      : 2; // bit 4-5
        uint8_t secondary_electric_brake_led : 2; // bit 6-7
    } data;
    uint8_t asByte;
};

union CarLedStatusByte3Struct {
    struct {
        uint8_t blind_spot_monitoring_led : 2; // bit 0-1
        uint8_t ac_airflow_type           : 2; // bit 2-3
        uint8_t fuel_info                 : 2; // bit 4-5
        uint8_t stop_start_led            : 2; // bit 6-7
    } data;
    uint8_t asByte;
};

union CarLedStatusByte4Struct {
    struct {
        uint8_t dynamic_led          : 2; // bit 0-1
        uint8_t stt_led              : 2; // bit 2-3
        uint8_t cafr_led             : 1; // bit 4
        uint8_t artiv_led            : 2; // bit 5-6
        uint8_t ac_airflow_recycling : 1; // bit 7
    } data;
    uint8_t asByte;
};