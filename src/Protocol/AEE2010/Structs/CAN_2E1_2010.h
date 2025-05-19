// CAN_2E1_2004.h
#pragma once

#ifndef _CAN_2E1_2010_h
    #define _CAN_2E1_2010_h

#include <stdint.h>

const uint16_t CAN_2E1_2010_INTERVAL = 1000;

// CANID: 2E1
const uint16_t CAN_ID_2E1_2010 = 0x2E1;

const uint8_t PARKING_AID_STATUS_FUNCTION_ABSENT_2010      = 0;
const uint8_t PARKING_AID_STATUS_NOT_ACTIVATED_2010        = 1;
const uint8_t PARKING_AID_STATUS_CODING_NOT_SPECIFIED_2010 = 2;
const uint8_t PARKING_AID_STATUS_ACTIVATED_2010            = 3;

const uint8_t PASSENGER_AIRBAG_STATUS_FUNCTION_ABSENT_2010      = 0;
const uint8_t PASSENGER_AIRBAG_STATUS_NOT_ACTIVATED_2010        = 1;
const uint8_t PASSENGER_AIRBAG_STATUS_CODING_NOT_SPECIFIED_2010 = 2;
const uint8_t PASSENGER_AIRBAG_STATUS_ACTIVATED_2010            = 3;

const uint8_t AUTO_HEADLIGHT_STATUS_FUNCTION_ABSENT_2010      = 0;
const uint8_t AUTO_HEADLIGHT_STATUS_NOT_ACTIVATED_2010        = 1;
const uint8_t AUTO_HEADLIGHT_STATUS_CODING_NOT_SPECIFIED_2010 = 2;
const uint8_t AUTO_HEADLIGHT_STATUS_ACTIVATED_2010            = 3;

const uint8_t AUTO_DOOR_LOCKING_STATUS_FUNCTION_ABSENT_2010      = 0;
const uint8_t AUTO_DOOR_LOCKING_STATUS_NOT_ACTIVATED_2010        = 1;
const uint8_t AUTO_DOOR_LOCKING_STATUS_CODING_NOT_SPECIFIED_2010 = 2;
const uint8_t AUTO_DOOR_LOCKING_STATUS_ACTIVATED_2010            = 3;

const uint8_t DOOR_LOCKING_STATUS_FUNCTION_ABSENT_2010      = 0;
const uint8_t DOOR_LOCKING_STATUS_NOT_ACTIVATED_2010        = 1;
const uint8_t DOOR_LOCKING_STATUS_CODING_NOT_SPECIFIED_2010 = 2;
const uint8_t DOOR_LOCKING_STATUS_ACTIVATED_2010            = 3;

const uint8_t ESP_STATUS_FUNCTION_ABSENT_2010      = 0;
const uint8_t ESP_STATUS_NOT_ACTIVATED_2010        = 1;
const uint8_t ESP_STATUS_CODING_NOT_SPECIFIED_2010 = 2;
const uint8_t ESP_STATUS_ACTIVATED_2010            = 3;

const uint8_t AUTO_WIPER_STATUS_FUNCTION_ABSENT_2010      = 0;
const uint8_t AUTO_WIPER_STATUS_NOT_ACTIVATED_2010        = 1;
const uint8_t AUTO_WIPER_STATUS_CODING_NOT_SPECIFIED_2010 = 2;
const uint8_t AUTO_WIPER_STATUS_ACTIVATED_2010            = 3;

const uint8_t SUSPENSION_STATUS_FUNCTION_ABSENT_2010      = 0;
const uint8_t SUSPENSION_STATUS_SPORT_2010                = 1;
const uint8_t SUSPENSION_STATUS_NORMAL_2010               = 2;
const uint8_t SUSPENSION_CODING_NOT_SPECIFIED_2010        = 3;

const uint8_t CHILD_SAFETY_STATUS_FUNCTION_ABSENT_2010      = 0;
const uint8_t CHILD_SAFETY_STATUS_NOT_ACTIVATED_2010        = 1;
const uint8_t CHILD_SAFETY_STATUS_CODING_NOT_SPECIFIED_2010 = 2;
const uint8_t CHILD_SAFETY_STATUS_ACTIVATED_2010            = 3;

const uint8_t START_STOP_STATUS_FUNCTION_ABSENT_2010      = 0;
const uint8_t START_STOP_STATUS_NOT_ACTIVATED_2010        = 1;
const uint8_t START_STOP_STATUS_CODING_NOT_SPECIFIED_2010 = 2;
const uint8_t START_STOP_STATUS_CODING_ACTIVATED_2010     = 3;

const uint8_t ROOF_STATUS_NO_DISPLAY_2010             = 0;
const uint8_t ROOF_STATUS_VEHICLE_IN_COUPE_2010       = 1;
const uint8_t ROOF_STATUS_BOOT_OPEN_ROOF_OPEN_2010    = 2;
const uint8_t ROOF_STATUS_BOOT_OPEN_ROOF_IN_BOOT_2010 = 3;
const uint8_t ROOF_STATUS_VEHICLE_IN_CONVERTIBLE_2010 = 4;
const uint8_t ROOF_STATUS_BOOT_OPEN_ROOF_CLOSED_2010  = 5;

const uint8_t SUSPENSION_INITIAL_POSITION_NORMAL_2010     = 0;
const uint8_t SUSPENSION_INITIAL_POSITION_MID_2010        = 1;
const uint8_t SUSPENSION_INITIAL_POSITION_LOW_2010        = 2;
const uint8_t SUSPENSION_INITIAL_POSITION_HIGH_2010       = 3;
const uint8_t SUSPENSION_INITIAL_POSITION_NO_DISPLAY_2010 = 7;

const uint8_t SUSPENSION_FINAL_POSITION_NORMAL_2010     = 0;
const uint8_t SUSPENSION_FINAL_POSITION_MID_2010        = 1;
const uint8_t SUSPENSION_FINAL_POSITION_LOW_2010        = 2;
const uint8_t SUSPENSION_FINAL_POSITION_HIGH_2010       = 3;
const uint8_t SUSPENSION_FINAL_POSITION_NO_DISPLAY_2010 = 7;

const uint8_t SUSPENSION_MOVEMENT_NO_MOVEMENT_2010 = 0;
const uint8_t SUSPENSION_MOVEMENT_RISE_2010        = 1;
const uint8_t SUSPENSION_MOVEMENT_DESCENT_2010     = 2;
const uint8_t SUSPENSION_MOVEMENT_REFUSED_2010      = 3;

const uint8_t SUSPENSION_ACTUAL_POSITION_NORMAL_2010     = 0;
const uint8_t SUSPENSION_ACTUAL_POSITION_MID_2010        = 1;
const uint8_t SUSPENSION_ACTUAL_POSITION_LOW_2010        = 2;
const uint8_t SUSPENSION_ACTUAL_POSITION_HIGH_2010       = 3;
const uint8_t SUSPENSION_ACTUAL_POSITION_NO_DISPLAY_2010 = 7;

// Read right to left in documentation
union CAN_2E1_2010_Byte1Struct {
    struct {
        uint8_t automatic_door_locking_status : 2; // bit 0-1
        uint8_t auto_headlight_status         : 2; // bit 2-3
        uint8_t passenger_airbag_status       : 2; // bit 4-5
        uint8_t parking_aid_status            : 2; // bit 6-7
    }data;
  uint8_t asByte;
};

// Read right to left in documentation
union CAN_2E1_2010_Byte2Struct {
    struct {
        uint8_t suspension_status   : 2; // bit 0-1
        uint8_t auto_wiper_status   : 2; // bit 2-3
        uint8_t esp_status          : 2; // bit 4-5
        uint8_t door_locking_status : 2; // bit 6-7
    }data;
  uint8_t asByte;
};

// Read right to left in documentation
union CAN_2E1_2010_Byte3Struct {
    struct {
        uint8_t push_start_stop_status : 1; // bit 0-1 (only AEE2010)
        uint8_t roof_status            : 3; // bit 2-3
        uint8_t start_stop_status      : 2; // bit 4-5
        uint8_t child_safety_status    : 2; // bit 6-7
    }data;
  uint8_t asByte;
};

// Read right to left in documentation
union CAN_2E1_2010_Byte4Struct {
    struct {
        uint8_t suspension_movement         : 2; // bit 0-1
        uint8_t final_suspension_position   : 3; // bit 2-4
        uint8_t initial_suspension_position : 3; // bit 5-7
    }data;
  uint8_t asByte;
};

// Read right to left in documentation
union CAN_2E1_2010_Byte5Struct {
    struct {
        uint8_t unused                     : 1; // bit 0
        uint8_t asr_plus                   : 3; // bit 1-3
        uint8_t suspension_alarm           : 1; // bit 4
        uint8_t actual_suspension_position : 3; // bit 5-7
    }data;
  uint8_t asByte;
};

union CAN_2E1_2010_Byte6Struct {
    struct {
        uint8_t asr_plus    : 4; // bit 0-3
        uint8_t unknown4    : 1; // bit 4
        uint8_t roof_status : 3; // bit 5-7
    }data;
  uint8_t asByte;
};

// Read left to right in documentation
struct CAN_2E1_2010Struct {
    CAN_2E1_2010_Byte1Struct Field1;
    CAN_2E1_2010_Byte2Struct Field2;
    CAN_2E1_2010_Byte3Struct Field3;
    CAN_2E1_2010_Byte4Struct Field4;
    CAN_2E1_2010_Byte5Struct Field5;
    CAN_2E1_2010_Byte6Struct Field6;
};
#endif
