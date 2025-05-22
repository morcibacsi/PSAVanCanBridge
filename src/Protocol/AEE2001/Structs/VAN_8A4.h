#pragma once

#ifndef _VAN_8A4_h
#define _VAN_8A4_h

#include <stdint.h>

// VANID: 8A4
const uint16_t VAN_ID_DASHBOARD = 0x8A4;

const uint8_t VAN_KEY_POSITION_STOP       = 0x00;
const uint8_t VAN_KEY_POSITION_ACCESORIES = 0x01;
const uint8_t VAN_KEY_POSITION_STARTER    = 0x10;
const uint8_t VAN_KEY_POSITION_CONTACT    = 0x11;

// Read right to left in documentation
typedef struct {
    // 0-7
    uint8_t brightness         : 4; // bit 0-3 (0-15)
    uint8_t wiper_status       : 1; // bit 4
    uint8_t unused             : 1; // bit 5
    uint8_t black_panel_status : 1; // bit 6
    uint8_t is_backlight_off   : 1; // bit 7 (1 when the sidelights are off)
} VanDashboardByte0Struct;

typedef struct {
    uint8_t key_position    : 2; // bit 0-1
    uint8_t engine_running  : 1; // bit 2
    uint8_t standby_mode    : 1; // bit 3
    uint8_t economy_mode    : 1; // bit 4
    uint8_t reverse_gear    : 1; // bit 5
    uint8_t factory_mode    : 1; // bit 6
    uint8_t alarm_active    : 1; // bit 7
} VanDashboardByte1Struct;

//Read left to right in documentation
struct VanDashboardStructs {
    VanDashboardByte0Struct Field0;
    VanDashboardByte1Struct Field1;
    uint8_t CoolantTemperature;
    uint8_t MileageByte1;
    uint8_t MileageByte2;
    uint8_t MileageByte3;
    uint8_t ExternalTemperature;
};
#endif