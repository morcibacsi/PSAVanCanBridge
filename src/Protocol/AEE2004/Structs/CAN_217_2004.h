// CAN_217.h
#pragma once

#ifndef _CAN_217_h
    #define _CAN_217_h

#include <stdint.h>

const uint16_t CAN_217_INTERVAL = 100;

// CANID: 217
const uint16_t CAN_ID_217 = 0x217;

// Read right to left in documentation
union CAN_217_Byte1Struct {
    struct {
        uint8_t airbag_disabled    : 2; // bit 0-1
        uint8_t reset_maintenance  : 1; // bit 2
        uint8_t black_panel        : 1; // bit 3
        uint8_t brightness         : 4; // bit 4-7
    }data;
  uint8_t asByte;
};

// Read right to left in documentation
union CAN_217_Byte2Struct {
    struct {
        uint8_t distance_unit : 1; // bit 0
        uint8_t reset_trip    : 1; // bit 1
        uint8_t reset_trip_1  : 1; // bit 2
        uint8_t cmb_fault     : 1; // bit 3
        uint8_t auto_check    : 1; // bit 4
        uint8_t check         : 1; // bit 5
        uint8_t airbag_fault  : 2; // bit 6-7
    }data;
  uint8_t asByte;
};

union CAN_217_Byte3Struct {
    struct {
        uint8_t rheostat_minus            : 1; // bit 0
        uint8_t rheostat_plus             : 1; // bit 1
        uint8_t partial_filtered_odometer : 1; // bit 2
        uint8_t matt_check_request        : 1; // bit 3
        uint8_t sport_push                : 1; // bit 4
        uint8_t child_lock_pushed         : 1; // bit 5
        uint8_t esp_pushed                : 1; // bit 6
        uint8_t aas_pushed                : 1; // bit 7
    }data;
  uint8_t asByte;
};

union CAN_217_Byte4Struct {
    struct {
        uint8_t reset_trip_2                : 1; // bit 0
        uint8_t compressor_displacement_req : 1; // bit 1
        uint8_t mpd_push                    : 1; // bit 2
        uint8_t blindspot_monitor_push      : 1; // bit 3
        uint8_t rear_window_heating_pushed  : 1; // bit 4
        uint8_t ac_on_pushed                : 1; // bit 5
        uint8_t stl_pushed                  : 1; // bit 6
        uint8_t overspeed_button_pushed     : 1; // bit 7
    }data;
  uint8_t asByte;
};

union CAN_217_Byte5Struct {
    struct {
        uint8_t stt_missing                 : 1; // bit 0
        uint8_t stt_pushed                  : 1; // bit 1
        uint8_t artiv_minus                 : 1; // bit 2
        uint8_t artiv_plus                  : 1; // bit 3
        uint8_t reset_maintenance_matt      : 1; // bit 4
        uint8_t artiv_push                  : 1; // bit 5
        uint8_t ptc_request                 : 1; // bit 6
        uint8_t stop_and_go_engine_stop     : 1; // bit 7
    }data;
  uint8_t asByte;
};

union CAN_217_Byte6Struct {
    struct {
        uint8_t level7_alerts_enabled : 1; // bit 0
        uint8_t level6_alerts_enabled : 1; // bit 1
        uint8_t level5_alerts_enabled : 1; // bit 2
        uint8_t level4_alerts_enabled : 1; // bit 3
        uint8_t level3_alerts_enabled : 1; // bit 4
        uint8_t level2_alerts_enabled : 1; // bit 5
        uint8_t level1_alerts_enabled : 1; // bit 6
        uint8_t level0_alerts_enabled : 1; // bit 7
    }data;
  uint8_t asByte;
};

union CAN_217_Byte7Struct {
    struct {
        uint8_t level15_alerts_enabled : 1; // bit 0
        uint8_t level14_alerts_enabled : 1; // bit 1
        uint8_t level13_alerts_enabled : 1; // bit 2
        uint8_t level12_alerts_enabled : 1; // bit 3
        uint8_t level11_alerts_enabled : 1; // bit 4
        uint8_t level10_alerts_enabled : 1; // bit 5
        uint8_t level9_alerts_enabled  : 1; // bit 6
        uint8_t level8_alerts_enabled  : 1; // bit 7
    }data;
  uint8_t asByte;
};

// Read left to right in documentation
struct CAN_217Struct {
    CAN_217_Byte1Struct Brightness;
    CAN_217_Byte2Struct Field2;
    CAN_217_Byte3Struct Field3;
    CAN_217_Byte4Struct Field4;
    CAN_217_Byte5Struct Field5;
    uint8_t SpeedDisplayedOnCmb;
    CAN_217_Byte6Struct Field7;
    CAN_217_Byte7Struct Field8;
};
#endif
