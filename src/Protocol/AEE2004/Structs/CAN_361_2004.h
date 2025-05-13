#pragma once

#ifndef _Can361_2004Struct_h
    #define _Can361_2004Struct_h

#include <stdint.h>

const uint16_t CAN_ID_361_2004_INTERVAL = 500;

// CANID: 0x361_2004
const uint16_t CAN_ID_361_2004 = 0x361;

// Read right to left in documentation
union Can361_2004Byte1Struct {
    struct {
        uint8_t profile_number        : 3; // bit 0-1
        uint8_t profil_change_enabled : 1; // bit 3
        uint8_t                       : 4; // bit 4-7
    }data;
  uint8_t asByte;
};

// Read right to left in documentation
union Can361_2004Byte2Struct {
    struct {
        uint8_t permanent_rear_flap_lock : 1; // bit 0
        uint8_t partial_windows          : 1; // bit 1
        uint8_t welcome_function         : 1; // bit 2
        uint8_t securoscope_enabled      : 1; // bit 3
        uint8_t config_enabled           : 1; // bit 4
        uint8_t                          : 3; // bit 5-7
    }data;
  uint8_t asByte;
};

// Read right to left in documentation
union Can361_2004Byte3Struct {
    struct {
        uint8_t auto_lighting                 : 1; // bit 0
        uint8_t irc_present                   : 1; // bit 1
        uint8_t automatic_electric_brake      : 1; // bit 2
        uint8_t motorway_lighting             : 1; // bit 3
        uint8_t follow_me_home                : 1; // bit 4
        uint8_t lock_on_coe                   : 1; // bit 5
        uint8_t auto_distance_closure_option  : 1; // bit 6
        uint8_t hinge_panel_select            : 1; // bit 7
    }data;
  uint8_t asByte;
};

// Read right to left in documentation
union Can361_2004Byte4Struct {
    struct {
        uint8_t                   : 1; // bit 0-4
        uint8_t rear_wiper_option : 1; // bit 5
        uint8_t drl_present       : 1; // bit 6
        uint8_t code_virage       : 1; // bit 7
    }data;
  uint8_t asByte;
};

// Read right to left in documentation
union Can361_2004Byte5Struct {
    struct {
        uint8_t disable_blindspot_monitoring  : 1; // bit 0
        uint8_t blindspot_monitoring          : 1; // bit 1
        uint8_t ambient_lighting              : 1; // bit 2
        uint8_t highway_lighting_present      : 1; // bit 3
        uint8_t emf_present                   : 1; // bit 4
        uint8_t aas_disable                   : 1; // bit 5
        uint8_t aas_audible_feedback          : 1; // bit 6
        uint8_t aas_visual_feedback           : 1; // bit 7
    }data;
  uint8_t asByte;
};

// Read right to left in documentation
union Can361_2004Byte6Struct {
    struct {
        uint8_t                                    : 1; // bit 0
        uint8_t breaking_on_alarm_risk             : 1; // bit 1
        uint8_t tpms_reset_present                 : 1; // bit 2
        uint8_t tnb_present                        : 1; // bit 3
        uint8_t tpms_present                       : 3; // bit 4-6
        uint8_t disable_blindspot_monitoring_sound : 1; // bit 7
    }data;
  uint8_t asByte;
};

// Read left to right in documentation
struct Can361_2004Struct {
    Can361_2004Byte1Struct Field1;
    Can361_2004Byte2Struct Field2;
    Can361_2004Byte3Struct Field3;
    Can361_2004Byte4Struct Field4;
    Can361_2004Byte5Struct Field5;
    Can361_2004Byte6Struct Field6;
};
#endif
