#pragma once

#ifndef _Can361_2010Struct_h
    #define _Can361_2010Struct_h

#include <stdint.h>

const uint16_t CAN_ID_361_2010_INTERVAL = 500;

// CANID: 0x361_2010
const uint16_t CAN_ID_361_2010 = 0x361;

// Read right to left in documentation
union Can361_2010Byte1Struct {
    struct {
        uint8_t drl_present                  : 1; // bit 0
        uint8_t auto_lighting                : 1; // bit 1
        uint8_t ambient_lighting             : 1; // bit 2
        uint8_t blindspot_monitoring         : 1; // bit 3
        uint8_t adaptive_lighting_present    : 1; // bit 4
        uint8_t unlock_lighting_customizable : 1; // bit 5
        uint8_t highway_lighting_present     : 1; // bit 6
        uint8_t setting_menu_availbale       : 1; // bit 7
    }data;
  uint8_t asByte;
};

// Read right to left in documentation
union Can361_2010Byte2Struct {
    struct {
        uint8_t cab_hinged_panel_select  : 1; // bit 0
        uint8_t hinge_panel_select       : 1; // bit 1
        uint8_t rr_hinged_panel_select   : 1; // bit 2
        uint8_t rear_flap_motorized      : 1; // bit 3
        uint8_t permanent_rear_flap_lock : 1; // bit 4
        uint8_t follow_me_home           : 1; // bit 5
        uint8_t rear_wiper_option        : 1; // bit 6
        uint8_t aas_disable              : 1; // bit 7
    }data;
  uint8_t asByte;
};

// Read right to left in documentation
union Can361_2010Byte3Struct {
    struct {
        uint8_t ppc_present                : 1; // bit 0
        uint8_t etsr_present               : 1; // bit 1
        uint8_t rear_driving_index_present : 1; // bit 2
        uint8_t sound_harmony_menu_present : 1; // bit 3
        uint8_t automatic_electric_brake   : 1; // bit 4
        uint8_t config_enabled             : 1; // bit 5
        uint8_t xvv_stops_present          : 1; // bit 6
        uint8_t tnb_present                : 1; // bit 7
    }data;
  uint8_t asByte;
};

// Read right to left in documentation
union Can361_2010Byte4Struct {
    struct {
        uint8_t tpms_present           : 3; // bit 0-2
        uint8_t irc_present            : 1; // bit 3
        uint8_t xvv_speed_menu         : 1; // bit 4
        uint8_t arc_sensitivity_menu   : 1; // bit 5
        uint8_t breaking_on_alarm_risk : 1; // bit 6
        uint8_t tpms_reset_present     : 1; // bit 7
    }data;
  uint8_t asByte;
};

// Read right to left in documentation
union Can361_2010Byte5Struct {
    struct {
        uint8_t access_arm_lock                 : 1; // bit 0
        uint8_t user_profil_present             : 1; // bit 1
        uint8_t access_arm_present              : 1; // bit 2
        uint8_t ilv_present                     : 1; // bit 3
        uint8_t radiator_grill_lighting_present : 1; // bit 4
        uint8_t charge_low_current              : 1; // bit 5
        uint8_t                                 : 2; // bit 6-7
    }data;
  uint8_t asByte;
};

// Read right to left in documentation
union Can361_2010Byte6Struct {
    struct {
        uint8_t irv_menu_enabled            : 1; // bit 0
        uint8_t ppc_block_enabled           : 1; // bit 1
        uint8_t ppc_anim_enabled            : 1; // bit 2
        uint8_t dae_4wd_present             : 1; // bit 3
        uint8_t dae_present                 : 1; // bit 4
        uint8_t cafr_enabled                : 1; // bit 5
        uint8_t see_menu_enabled            : 1; // bit 6
        uint8_t driver_alert_assist_enabled : 1; // bit 7
    }data;
  uint8_t asByte;
};

// Read right to left in documentation
union Can361_2010Byte7Struct {
    struct {
        uint8_t                      : 1; // bit 0
        uint8_t rtab_enabled         : 1; // bit 1
        uint8_t ac_menu_enabled      : 1; // bit 2
        uint8_t amla_enabled         : 1; // bit 3
        uint8_t adsd_enabled         : 1; // bit 4
        uint8_t privacy_mode_enabled : 1; // bit 5
        uint8_t ars_settings_enabled : 1; // bit 6
        uint8_t afs_settings_enabled : 1; // bit 7
    }data;
  uint8_t asByte;
};

// Read left to right in documentation
struct Can361_2010Struct {
    Can361_2010Byte1Struct Field1;
    Can361_2010Byte2Struct Field2;
    Can361_2010Byte3Struct Field3;
    Can361_2010Byte4Struct Field4;
    Can361_2010Byte5Struct Field5;
    Can361_2010Byte6Struct Field6;
    Can361_2010Byte6Struct Field7;
};
#endif
