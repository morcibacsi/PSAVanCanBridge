#pragma once

#ifndef _2004_0E8_h
    #define _2004_0E8_h

#include <stdint.h>

// CANID: 0E8
const uint16_t CAN_ID_0E8 = 0x0E8;
const uint8_t CAN_ID_0E8_LEN = 7;

const uint8_t VEHICLE_TYPE_5_DOORS = 0;
const uint8_t VEHICLE_TYPE_3_DOORS = 1;

const uint8_t body_movement_nature_none       = 0b00;
const uint8_t body_movement_nature_ascending  = 0b01;
const uint8_t body_movement_nature_descending = 0b10;
const uint8_t body_movement_nature_refused    = 0b11;

const uint8_t body_position_normal   = 0b000;
const uint8_t body_position_mid_high = 0b001;
const uint8_t body_position_low      = 0b010;
const uint8_t body_position_high     = 0b011;
const uint8_t body_position_unused1  = 0b100;
const uint8_t body_position_unused2  = 0b101;
const uint8_t body_position_unused3  = 0b110;
const uint8_t body_position_nonte    = 0b111;

// Read right to left in documentation
union CAN_0E8_Byte1Struct {
    struct {
        uint8_t hood_open                 : 1; // bit 0
        uint8_t rear_left_door_open       : 1; // bit 1
        uint8_t rear_right_door_open      : 1; // bit 2
        uint8_t front_left_door_open      : 1; // bit 3
        uint8_t front_right_door_open     : 1; // bit 4
        uint8_t door_alert_above_10kmph   : 1; // bit 5
        uint8_t door_alert_below_10kmph   : 1; // bit 6
        uint8_t key_alert                 : 1; // bit 7
    }data;
  uint8_t asByte;
};

union CAN_0E8_Byte2Struct {
    struct {
        uint8_t esp_alert_enabled               : 1; // bit 0
        uint8_t handbrake_on_while_accelerating : 1; // bit 1
        uint8_t fuel_flap_open_below_10kmph     : 1; // bit 2
        uint8_t bva_safety_alert                : 1; // bit 3
        uint8_t vehicle_type                    : 1; // bit 4
        uint8_t fuel_flap_open_above_10kmph     : 1; // bit 5
        uint8_t rear_window_open                : 1; // bit 6
        uint8_t trunk_open                      : 1; // bit 7
    }data;
  uint8_t asByte;
};

union CAN_0E8_Byte3Struct {
    struct {
        uint8_t final_body_position       : 3; // bit 0-2
        uint8_t body_movement_nature      : 2; // bit 3-4
        uint8_t suspension_movement_alert : 1; // bit 5
        uint8_t foot_on_brake_alert       : 1; // bit 6
        uint8_t esp_alert_disabled        : 1; // bit 7
    }data;
  uint8_t asByte;
};

union CAN_0E8_Byte4Struct {
    struct {
        uint8_t off_suspension_alert  : 1; // bit 0
        uint8_t on_suspension_alert   : 1; // bit 1
        uint8_t real_body_position    : 3; // bit 2-4
        uint8_t initial_body_position : 3; // bit 5-7
    }data;
  uint8_t asByte;
};

union CAN_0E8_Byte5Struct {
    struct {
        uint8_t child_safety_deactivated          : 1; // bit 0
        uint8_t lock_alert_while_driving_disabled : 1; // bit 1
        uint8_t lock_alert_while_driving_enabled  : 1; // bit 2
        uint8_t windscreen_washer_level_low       : 1; // bit 3
        uint8_t auto_wiper_alert_disabled         : 1; // bit 4
        uint8_t auto_wiper_alert_enabled          : 1; // bit 5
        uint8_t auto_lighting_alert_disabled      : 1; // bit 6
        uint8_t auto_lighting_alert_enabled       : 1; // bit 7
    }data;
  uint8_t asByte;
};

union CAN_0E8_Byte6Struct {
    struct {
        uint8_t child_safety_activated       : 1; // bit 0
        uint8_t lights_reminder_alert        : 1; // bit 1
        uint8_t lvv_rvv_alert                : 1; // bit 2
        uint8_t rvv_alert_speed_too_low      : 1; // bit 3
        uint8_t rvv_alert_speed_not_detected : 1; // bit 4
        uint8_t mpv_alert_in_progress        : 1; // bit 5
        uint8_t alert_frame_invalid          : 1; // bit 6
        uint8_t unused7                      : 1; // bit 7
    }data;
  uint8_t asByte;
};


struct Can0E8Struct {
    CAN_0E8_Byte1Struct Field1;
    CAN_0E8_Byte2Struct Field2;
    CAN_0E8_Byte3Struct Field3;
    CAN_0E8_Byte4Struct Field4;
    CAN_0E8_Byte5Struct Field5;
    CAN_0E8_Byte6Struct Field6;
    uint8_t Unused7;
};

union Can0E8Packet {
    Can0E8Struct data;
    uint8_t Can0E8Packet[sizeof(Can0E8Struct)];
};
#endif
