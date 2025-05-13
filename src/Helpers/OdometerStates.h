#pragma once

#include <inttypes.h>

union OdometerStatesStruct {
    struct {
        uint8_t airbag_disabled    : 2; // bit 0-1
        uint8_t reset_maintenance  : 1; // bit 2
        uint8_t black_panel        : 1; // bit 3
        uint8_t brightness         : 4; // bit 4-7

        uint8_t distance_unit : 1; // bit 0
        uint8_t reset_trip    : 1; // bit 1
        uint8_t reset_trip_1  : 1; // bit 2
        uint8_t cmb_fault     : 1; // bit 3
        uint8_t auto_check    : 1; // bit 4
        uint8_t check         : 1; // bit 5
        uint8_t airbag_fault  : 2; // bit 6-7

        uint8_t unused                    : 2; // bit 0-1
        uint8_t partial_filtered_odometer : 1; // bit 2
        uint8_t matt_check_request        : 1; // bit 3
        uint8_t sport_push                : 1; // bit 4
        uint8_t child_lock_pushed         : 1; // bit 5
        uint8_t esp_pushed                : 1; // bit 6
        uint8_t aas_pushed                : 1; // bit 7

        uint8_t reset_trip_2                : 1; // bit 0
        uint8_t compressor_displacement_req : 1; // bit 1
        uint8_t mpd_push                    : 1; // bit 2
        uint8_t blindspot_monitor_push      : 1; // bit 3
        uint8_t rear_window_heating_pushed  : 1; // bit 4
        uint8_t ac_on_pushed                : 1; // bit 5
        uint8_t stl_pushed                  : 1; // bit 6
        uint8_t cafr_pushed                 : 1; // bit 7

        uint8_t scp_state_change       : 1; // bit 0
        uint8_t media_fault            : 1; // bit 1
        uint8_t artiv_minus            : 1; // bit 2
        uint8_t artiv_plus             : 1; // bit 3
        uint8_t reset_maintenance_matt : 1; // bit 4
        uint8_t artiv_push             : 1; // bit 5
        uint8_t ctp_request            : 1; // bit 6
        uint8_t dsgi_push              : 1; // bit 7

        uint8_t SpeedDisplayedOnCmb;

        uint8_t ambience_level         : 3; // bit 0-2
        uint8_t ambience_lighting_push : 1; // bit 3
        uint8_t media_display_fault    : 1; // bit 4
        uint8_t dynamic_mode_request   : 1; // bit 5
        uint8_t eco_push               : 1; // bit 6
        uint8_t stt_push               : 1; // bit 7
    } data;
    uint64_t asUint64;
};