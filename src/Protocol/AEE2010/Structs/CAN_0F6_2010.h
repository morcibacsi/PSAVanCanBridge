// CanDash1Structs.h
#pragma once

#ifndef _CanDash1Structs_2010_h
    #define _CanDash1Structs_2010_h

#include <stdint.h>

const uint16_t CAN_ID_DASH1_2010_INTERVAL = 500;

// CANID: 0F6
const uint16_t CAN_ID_0F6_2010_DASH = 0x0F6;

const uint8_t TYPE_OF_DRIVE_2010_INVALID  = 0b00;
const uint8_t TYPE_OF_DRIVE_2010_RIGHT    = 0b01;
const uint8_t TYPE_OF_DRIVE_2010_LEFT     = 0b10;
const uint8_t TYPE_OF_DRIVE_2010_INVALID2 = 0b11;

const uint8_t KEY_POSITION_2010_STOP    = 0b00;
const uint8_t KEY_POSITION_2010_CONTACT = 0b01;
const uint8_t KEY_POSITION_2010_STARTER = 0b10;
const uint8_t KEY_POSITION_2010_FREE    = 0b11;

const uint8_t CONFIG_MODE_2010_FACTORY = 0b00;
const uint8_t CONFIG_MODE_2010_FREE1   = 0b01;
const uint8_t CONFIG_MODE_2010_CLIENT  = 0b10;
const uint8_t CONFIG_MODE_2010_FREE2   = 0b11;

/*
const uint8_t CAN_INDICATOR_LEFT  = 121; //y
const uint8_t CAN_INDICATOR_RIGHT = 120; //x

const uint8_t CAN_KEY_POSITION_STOP       = 0x00;
const uint8_t CAN_KEY_POSITION_CONTACT    = 0x01;
const uint8_t CAN_KEY_POSITION_STARTER    = 0x10;
*/
// Read right to left in documentation
union CanDash1_2010_Byte1Struct {
    struct {
        uint8_t engine_status     : 2; // bit 0-1
        uint8_t generator_status  : 1; // bit 2
        uint8_t key_position      : 1; // bit 3-4
        uint8_t factory_mode      : 1; // bit 5
        uint8_t config_mode       : 2; // bit 6-7
    }data;
  uint8_t asByte;
};

union CanDash1_2010_Byte8Struct {
    struct {
        uint8_t turn_left_light    : 1; // bit 0
        uint8_t turn_right_light   : 1; // bit 1
        uint8_t unknown            : 1; // bit 2
        uint8_t cmb_needle_swipe   : 1; // bit 3
        uint8_t type_of_drive      : 2; // bit 4-5
        uint8_t wiper_status       : 1; // bit 6
        uint8_t reverse_gear_light : 1; // bit 7
    }data;
  uint8_t asByte;
};

// Read left to right in documentation
struct Can0F6Dash1_2010_Struct {
    CanDash1_2010_Byte1Struct IgnitionField;
    uint8_t CoolantTemperature;
    uint8_t MileageByte1;
    uint8_t MileageByte2;
    uint8_t MileageByte3;
    uint8_t Field6;
    uint8_t ExternalTemperature;
    CanDash1_2010_Byte8Struct LightsStatus;
};
#endif
