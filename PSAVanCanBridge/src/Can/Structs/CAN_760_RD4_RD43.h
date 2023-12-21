// CAN_760_RD4_RD43.h
#pragma once

#ifndef _CAN_760_RD4_RD43_h
    #define _CAN_760_RD4_RD43_h

#include <stdint.h>

const uint16_t CAN_760_RD4_RD43_INTERVAL = 100;

// CANID: 760_RD4_RD43
const uint16_t CAN_ID_760_RD4_RD43 = 0x760;

// Read right to left in documentation
union CAN_760_RD4_RD43_Byte1Struct {
    struct {
        uint8_t volume_auto_control       : 1; // bit 0
        uint8_t am_waveband_activated     : 1; // bit 1
        uint8_t rear_fader_activated      : 1; // bit 2
        uint8_t cd_changer_or_usb_enabled : 1; // bit 3
        uint8_t geographic_location       : 4; // bit 4-7
    }data;
  uint8_t asByte;
};

union CAN_760_RD4_RD43_Byte2Struct {
    struct {
        uint8_t aerial_failure_detection_activated  : 1; // bit 0
        uint8_t lo_dc_sensitivity_curve             : 2; // bit 1-2
        uint8_t volume_level_correction_pattern     : 3; // bit 3-5
        uint8_t unknown6                            : 1; // bit 6
        uint8_t amplifier_enabled                   : 1; // bit 7
    }data;
  uint8_t asByte;
};

union CAN_760_RD4_RD43_Byte3Struct {
    struct {
        uint8_t unknown0                     : 1; // bit 0
        uint8_t unknown1                     : 1; // bit 1
        uint8_t parking_aid_enabled          : 1; // bit 2
        uint8_t cd_text_activated            : 1; // bit 3
        uint8_t radio_text_activated         : 1; // bit 4
        uint8_t type_of_aerial_no2_activated : 1; // bit 5
        uint8_t type_of_aerial_no1_activated : 1; // bit 6
        uint8_t number_of_aerials            : 1; // bit 7
    }data;
  uint8_t asByte;
};

union CAN_760_RD4_RD43_Byte4Struct {
    struct {
        uint8_t unknown0                 : 1; // bit 0
        uint8_t unknown1                 : 1; // bit 1
        uint8_t unknown2                 : 1; // bit 2
        uint8_t is_amplifier_multiplexed : 1; // bit 3 (also changes bit 7 in byte2)
        uint8_t aux2_mode                : 2; // bit 4-5
        uint8_t aux1_mode                : 2; // bit 5-6
    }data;
  uint8_t asByte;
};

union CAN_760_RD4_RD43_Byte5Struct {
    struct {
        uint8_t rear_parking_beep_volume  : 3; // bit 0-2 (value: 0-7)
        uint8_t front_parking_beep_volume : 3; // bit 3-5 (value: 0-7)
        uint8_t unknown6                  : 1; // bit 6
        uint8_t unknown7                  : 1; // bit 7
    }data;
  uint8_t asByte;
};

// Read left to right in documentation
struct CAN_760_RD4_RD43_OptionsStruct {
    CAN_760_RD4_RD43_Byte1Struct Field1;
    CAN_760_RD4_RD43_Byte2Struct Field2;
    CAN_760_RD4_RD43_Byte3Struct Field3;
    CAN_760_RD4_RD43_Byte4Struct Field4;
    CAN_760_RD4_RD43_Byte5Struct Field5;
};
#endif
