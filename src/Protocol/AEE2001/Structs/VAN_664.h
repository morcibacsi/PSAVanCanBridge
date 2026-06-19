#pragma once

#ifndef _VAN_664_h
#define _VAN_664_h

#include <stdint.h>

// VANID: 8A4
const uint16_t VAN_ID_664 = 0x664;

// Read right to left in documentation
union VAN_664_Byte1Struct {
    struct {
        uint8_t rear_heating_status : 1; // bit 0
        uint8_t warning_push_status : 1; // bit 1
        uint8_t odometer_enable     : 1; // bit 2
        uint8_t side_airbag_fault   : 1; // bit 3
        uint8_t front_airbag_fault  : 1; // bit 4
        uint8_t coolant_level_alert : 1; // bit 5
        uint8_t oil_pressure_alert  : 1; // bit 6
        uint8_t oil_level_alert     : 1; // bit 7
    }data;
  uint8_t asByte;
};

union VAN_664_Byte2Struct {
    struct {
        uint8_t check_in_progress         : 1; // bit 0
        uint8_t push2_status              : 1; // bit 1
        uint8_t push1_status              : 1; // bit 2
        uint8_t fuel_flap_opened          : 1; // bit 3
        uint8_t handbrake_engaged         : 1; // bit 4
        uint8_t driver_seatbelt_forgotten : 1; // bit 5
        uint8_t black_panel_status        : 1; // bit 6
        uint8_t ac_on_push_active         : 1; // bit 7
    }data;
  uint8_t asByte;
};

union VAN_664_Byte3Struct {
    struct {
        uint8_t dashboard_brightness       : 4; // bit 0-3
        uint8_t maintenance_reset          : 1; // bit 4
        uint8_t alarm_push                 : 1; // bit 5
        uint8_t central_lock_push          : 1; // bit 6
        uint8_t hydractive_suspension_push : 1; // bit 7
    }data;
  uint8_t asByte;
};

union VAN_664_Byte4Struct {
    struct {
        uint8_t brake_pads_worn    : 1; // bit 0
        uint8_t esp_push           : 1; // bit 1
        uint8_t cmb_ready          : 1; // bit 2
        uint8_t auto_gearbox_fault : 1; // bit 3
        uint8_t brake_fluid_fault  : 1; // bit 4
        uint8_t ref_fault          : 1; // bit 5
        uint8_t abs_fault          : 1; // bit 6
        uint8_t fuel_level_minimum : 1; // bit 7
    }data;
  uint8_t asByte;
};

union VAN_664_Byte7Struct {
    struct {
        uint8_t overspeed_alert_push           : 1; // bit 0
        uint8_t conda_active                   : 1; // bit 1
        uint8_t overspeed_alert                : 1; // bit 2
        uint8_t airbag_light_inhibition_status : 1; // bit 3
        uint8_t rear_foglight_status           : 1; // bit 4
        uint8_t front_foglight_status          : 1; // bit 5
        uint8_t high_beam_status               : 1; // bit 6
        uint8_t low_beam_status                : 1; // bit 7
    }data;
  uint8_t asByte;
};

union VAN_664_Byte11Struct {
    struct {
        uint8_t stop_light                 : 1; // bit 0
        uint8_t unknown                    : 1; // bit 1
        uint8_t                            : 1; // bit 2
        uint8_t fuel_switching_in_progress : 1; // bit 3
        uint8_t lpg_refused                : 1; // bit 4
        uint8_t lpg_fault                  : 1; // bit 5
        uint8_t lpg_combustion_active      : 1; // bit 6
        uint8_t lpg_fuel_level_minimum     : 1; // bit 7
    }data;
  uint8_t asByte;
};

//Read left to right in documentation
struct VAN_664_2001Struct {
    VAN_664_Byte1Struct Field0;
    VAN_664_Byte2Struct Field1;
    VAN_664_Byte3Struct Field2;
    VAN_664_Byte4Struct Field3;

    uint8_t FuelLevelFiltered;
    uint8_t FuelLevelUnfiltered;

    VAN_664_Byte7Struct Field6;
    uint8_t DisplayIcon;
    uint8_t CoolantLevel;
    uint8_t LPGFuelLevel;
    VAN_664_Byte11Struct Field10;
};
#endif