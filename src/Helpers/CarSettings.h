#pragma once

#ifndef _CarSettings_h
    #define _CarSettings_h

#include <cstdint>

// The file contains the bytes as they are used in AEE2004 messages: 0x15B, 0x260, 0x1DB

const uint8_t CONFIGURABLE_BUTTON_2004_CEILING_LIGHT = 0b0000;
const uint8_t CONFIGURABLE_BUTTON_2004_BLACK_PANEL   = 0b0001;
const uint8_t CONFIGURABLE_BUTTON_2004_ALERT_LOG     = 0b0010;
const uint8_t CONFIGURABLE_BUTTON_2004_CUSTOMIZATION = 0b0011;
const uint8_t CONFIGURABLE_BUTTON_2004_CMB_COLOR     = 0b0100;
const uint8_t CONFIGURABLE_BUTTON_2004_MANUAL_CHECK  = 0b0101;
const uint8_t CONFIGURABLE_BUTTON_2004_NONE          = 0b1111;

const uint8_t CONFIGURABLE_BUTTON_2010_FUNCTION_STATUS          = 0b0000;
const uint8_t CONFIGURABLE_BUTTON_2010_BLACK_PANEL              = 0b0001;
const uint8_t CONFIGURABLE_BUTTON_2010_CEILING_LIGHT            = 0b0010;
const uint8_t CONFIGURABLE_BUTTON_2010_PROFILE_MENU             = 0b0011;
const uint8_t CONFIGURABLE_BUTTON_2010_PROFILE_MENU_START_PAGE  = 0b0100;
const uint8_t CONFIGURABLE_BUTTON_2010_ALERT_LOG                = 0b0101;
const uint8_t CONFIGURABLE_BUTTON_2010_CUSTOMIZATION            = 0b0110;
const uint8_t CONFIGURABLE_BUTTON_2010_CMB_CONFIG               = 0b1000;
const uint8_t CONFIGURABLE_BUTTON_2010_CMB_COLOR                = 0b1001;
const uint8_t CONFIGURABLE_BUTTON_2010_MANUAL_CHECK             = 0b1010;
const uint8_t CONFIGURABLE_BUTTON_2010_NONE                     = 0b1111;

union CarSettings_Byte1Struct{
    struct {
        uint8_t profile_number : 2; // bit 1
        uint8_t data_valid     : 1; // bit 2
        uint8_t menu_visible   : 1; // bit 3
        uint8_t unknown47      : 4; // bit 4-7
    } data;
    uint8_t asByte;
};

union CarSettings_Byte2Struct{
    struct {
        uint8_t automatic_parking_brake                : 1; // bit 0
        uint8_t driver_welcome                         : 1; // bit 1
        uint8_t partial_window_drop                    : 1; // bit 2
        uint8_t locking_mode                           : 1; // bit 3
        uint8_t automatic_door_lock_when_leave_the_car : 1; // bit 4
        uint8_t trunk_is_closed_until_directly_opened  : 1; // bit 5
        uint8_t unused6                                : 1; // bit 6
        uint8_t selective_opening_key                  : 1; // bit 7
    } data;
    uint8_t asByte;
};

union CarSettings_Byte3Struct{
    struct {
        uint8_t follow_me_home_time          : 4; // bit 0-3 0: 0 seconds, 1: 15 seconds, 2: 30 seconds, 3: 45 seconds, 4: 60 seconds
        uint8_t automatic_headlights_enabled : 1; // bit 4
        uint8_t follow_me_home_enabled       : 1; // bit 5
        uint8_t unused6                      : 1; // bit 6
        uint8_t adaptive_lights              : 1; // bit 7
    } data;
    uint8_t asByte;
};

union CarSettings_Byte4Struct{
    struct {
        uint8_t highway_beam_enabled                   : 1; // bit 0
        uint8_t driver_welcome_exterior                : 1; // bit 1
        uint8_t unused25                               : 4; // bit 2-5
        uint8_t daylight_running_light_enabled         : 1; // bit 6
        uint8_t ambience_light                         : 1; // bit 7
    } data;
    uint8_t asByte;
};

union CarSettings_Byte5Struct{
    struct {
        uint8_t unused : 8; // bit 0-7
    } data;
    uint8_t asByte;
};

union CarSettings_Byte6Struct{
    struct {
        uint8_t unused03                               : 4; // bit 0-3
        uint8_t auto_rear_wiper_enabled                : 1; // bit 4
        uint8_t unused6                                : 1; // bit 5
        uint8_t aas_status                             : 2; // bit 6-7  0: none, 1: sound, 2: visual, 3: sound and visual
    } data;
    uint8_t asByte;
};

union CarSettings_Byte7Struct{
    struct {
        uint8_t automatic_door_lock_when_leave_the_car : 5; // bit 0-4
        uint8_t unused6                                : 1; // bit 5-6
        uint8_t selective_opening                      : 1; // bit 7
    } data;
    uint8_t asByte;
};

union CarSettings_Byte8Struct{
    struct {
        uint8_t unused02                      : 3; // bit 0-2
        uint8_t dsg_enabled                   : 1; // bit 3
        uint8_t configurable_button_function  : 4; // bit 4-7
    } data;
    uint8_t asByte;
};

struct CarSettings_Struct {
    CarSettings_Byte1Struct Field1;
    CarSettings_Byte2Struct Field2;
    CarSettings_Byte3Struct Field3;
    CarSettings_Byte4Struct Field4;
    CarSettings_Byte5Struct Field5;
    CarSettings_Byte6Struct Field6;
    CarSettings_Byte7Struct Field7;
    CarSettings_Byte8Struct Field8;
    uint8_t ProfileNumber2010 = 0;
    uint8_t FollowMeHomeTime2010 = 0;
    uint8_t ConfigurableButtonFunction2010 = 0;
};
#endif
