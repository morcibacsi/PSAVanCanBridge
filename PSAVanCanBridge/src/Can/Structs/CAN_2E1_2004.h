// CAN_2E1_2004.h
#pragma once

#ifndef _CAN_2E1_2004_h
    #define _CAN_2E1_2004_h

#include <stdint.h>

const uint16_t CAN_2E1_2004_INTERVAL = 1000;

// CANID: 2E1
const uint16_t CAN_ID_2E1_2004 = 0x2E1;

const uint8_t PARKING_AID_STATUS_FUNCTION_ABSENT      = 0;
const uint8_t PARKING_AID_STATUS_NOT_ACTIVATED        = 1;
const uint8_t PARKING_AID_STATUS_CODING_NOT_SPECIFIED = 2;
const uint8_t PARKING_AID_STATUS_ACTIVATED            = 3;

const uint8_t PASSENGER_AIRBAG_STATUS_FUNCTION_ABSENT      = 0;
const uint8_t PASSENGER_AIRBAG_STATUS_NOT_ACTIVATED        = 1;
const uint8_t PASSENGER_AIRBAG_STATUS_CODING_NOT_SPECIFIED = 2;
const uint8_t PASSENGER_AIRBAG_STATUS_ACTIVATED            = 3;

const uint8_t AUTO_HEADLIGHT_STATUS_FUNCTION_ABSENT      = 0;
const uint8_t AUTO_HEADLIGHT_STATUS_NOT_ACTIVATED        = 1;
const uint8_t AUTO_HEADLIGHT_STATUS_CODING_NOT_SPECIFIED = 2;
const uint8_t AUTO_HEADLIGHT_STATUS_ACTIVATED     = 3;

const uint8_t AUTO_DOOR_LOCKING_STATUS_FUNCTION_ABSENT      = 0;
const uint8_t AUTO_DOOR_LOCKING_STATUS_NOT_ACTIVATED        = 1;
const uint8_t AUTO_DOOR_LOCKING_STATUS_CODING_NOT_SPECIFIED = 2;
const uint8_t AUTO_DOOR_LOCKING_STATUS_ACTIVATED     = 3;

const uint8_t DOOR_LOCKING_STATUS_FUNCTION_ABSENT      = 0;
const uint8_t DOOR_LOCKING_STATUS_NOT_ACTIVATED        = 1;
const uint8_t DOOR_LOCKING_STATUS_CODING_NOT_SPECIFIED = 2;
const uint8_t DOOR_LOCKING_STATUS_ACTIVATED            = 3;

const uint8_t ESP_STATUS_FUNCTION_ABSENT      = 0;
const uint8_t ESP_STATUS_NOT_ACTIVATED        = 1;
const uint8_t ESP_STATUS_CODING_NOT_SPECIFIED = 2;
const uint8_t ESP_STATUS_ACTIVATED            = 3;

const uint8_t AUTO_WIPER_STATUS_FUNCTION_ABSENT      = 0;
const uint8_t AUTO_WIPER_STATUS_NOT_ACTIVATED        = 1;
const uint8_t AUTO_WIPER_STATUS_CODING_NOT_SPECIFIED = 2;
const uint8_t AUTO_WIPER_STATUS_ACTIVATED            = 3;

const uint8_t SUSPENSION_STATUS_FUNCTION_ABSENT      = 0;
const uint8_t SUSPENSION_STATUS_SPORT                = 1;
const uint8_t SUSPENSION_STATUS_NORMAL               = 2;
const uint8_t SUSPENSION_CODING_NOT_SPECIFIED        = 3;

const uint8_t CHILD_SAFETY_STATUS_FUNCTION_ABSENT      = 0;
const uint8_t CHILD_SAFETY_STATUS_NOT_ACTIVATED        = 1;
const uint8_t CHILD_SAFETY_STATUS_CODING_NOT_SPECIFIED = 2;
const uint8_t CHILD_SAFETY_STATUS_ACTIVATED            = 3;

const uint8_t START_STOP_STATUS_FUNCTION_ABSENT      = 0;
const uint8_t START_STOP_STATUS_NOT_ACTIVATED        = 1;
const uint8_t START_STOP_STATUS_CODING_NOT_SPECIFIED = 2;
const uint8_t START_STOP_STATUS_CODING_ACTIVATED     = 3;

const uint8_t ROOF_STATUS_NO_DISPLAY             = 0;
const uint8_t ROOF_STATUS_VEHICLE_IN_COUPE       = 1;
const uint8_t ROOF_STATUS_BOOT_OPEN_ROOF_OPEN    = 2;
const uint8_t ROOF_STATUS_BOOT_OPEN_ROOF_IN_BOOT = 3;
const uint8_t ROOF_STATUS_VEHICLE_IN_CONVERTIBLE = 4;
const uint8_t ROOF_STATUS_BOOT_OPEN_ROOF_CLOSED  = 5;

const uint8_t SUSPENSION_INITIAL_POSITION_NORMAL     = 0;
const uint8_t SUSPENSION_INITIAL_POSITION_MID        = 1;
const uint8_t SUSPENSION_INITIAL_POSITION_LOW        = 2;
const uint8_t SUSPENSION_INITIAL_POSITION_HIGH       = 3;
const uint8_t SUSPENSION_INITIAL_POSITION_NO_DISPLAY = 7;

const uint8_t SUSPENSION_FINAL_POSITION_NORMAL     = 0;
const uint8_t SUSPENSION_FINAL_POSITION_MID        = 1;
const uint8_t SUSPENSION_FINAL_POSITION_LOW        = 2;
const uint8_t SUSPENSION_FINAL_POSITION_HIGH       = 3;
const uint8_t SUSPENSION_FINAL_POSITION_NO_DISPLAY = 7;

const uint8_t SUSPENSION_MOVEMENT_NO_MOVEMENT = 0;
const uint8_t SUSPENSION_MOVEMENT_RISE = 1;
const uint8_t SUSPENSION_MOVEMENT_DESCENT = 2;
const uint8_t SUSPENSION_MOVEMENT_REFUSED = 3;

const uint8_t SUSPENSION_ACTUAL_POSITION_NORMAL     = 0;
const uint8_t SUSPENSION_ACTUAL_POSITION_MID        = 1;
const uint8_t SUSPENSION_ACTUAL_POSITION_LOW        = 2;
const uint8_t SUSPENSION_ACTUAL_POSITION_HIGH       = 3;
const uint8_t SUSPENSION_ACTUAL_POSITION_NO_DISPLAY = 7;

// Read right to left in documentation
union CAN_2E1_2004_Byte1Struct {
    struct {
        uint8_t automatic_door_locking_status : 2; // bit 0-1
        uint8_t auto_headlight_status         : 2; // bit 2-3
        uint8_t passenger_airbag_status       : 2; // bit 4-5
        uint8_t parking_aid_status            : 2; // bit 6-7
    }data;
  uint8_t asByte;
};

// Read right to left in documentation
union CAN_2E1_2004_Byte2Struct {
    struct {
        uint8_t suspension_status   : 2; // bit 0-1
        uint8_t auto_wiper_status   : 2; // bit 2-3
        uint8_t esp_status          : 2; // bit 4-5
        uint8_t door_locking_status : 2; // bit 6-7
    }data;
  uint8_t asByte;
};

// Read right to left in documentation
union CAN_2E1_2004_Byte3Struct {
    struct {
        uint8_t push_start_stop_status : 1; // bit 0-1 (only AEE2010)
        uint8_t roof_status            : 3; // bit 2-3
        uint8_t start_stop_status      : 2; // bit 4-5
        uint8_t child_safety_status    : 2; // bit 6-7
    }data;
  uint8_t asByte;
};

// Read right to left in documentation
union CAN_2E1_2004_Byte4Struct {
    struct {
        uint8_t suspension_movement         : 2; // bit 0-1
        uint8_t final_suspension_position   : 3; // bit 2-4
        uint8_t initial_suspension_position : 3; // bit 5-7
    }data;
  uint8_t asByte;
};

// Read right to left in documentation
union CAN_2E1_2004_Byte5Struct {
    struct {
        uint8_t unused                     : 4; // bit 0-3
        uint8_t suspension_alarm           : 1; // bit 4
        uint8_t actual_suspension_position : 3; // bit 5-7
    }data;
  uint8_t asByte;
};

// Read left to right in documentation
struct CAN_217_2004Struct {
    CAN_2E1_2004_Byte1Struct Brightness;
    CAN_2E1_2004_Byte2Struct Field2;
    CAN_2E1_2004_Byte3Struct Field3;
    CAN_2E1_2004_Byte4Struct Field4;
    CAN_2E1_2004_Byte5Struct Field5;
};
#endif
