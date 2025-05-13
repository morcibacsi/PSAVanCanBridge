// CAN_10B.h
#pragma once

#ifndef _CAN_10B_2010_h
    #define _CAN_10B_2010_h

#include <stdint.h>

const uint16_t CAN_10B_INTERVAL = 100;

// CANID: 10B
const uint16_t CAN_ID_DASH1 = 0x10B;

const uint8_t CAN_ROTATE_COUNTER_CLOCKWISE = 0;
const uint8_t CAN_ROTATE_CLOCKWISE = 1;

// Read right to left in documentation
union CAN_10B_Byte4Struct {
    struct {
        uint8_t intenal_status     : 1; // bit 0
        uint8_t is_calibrated      : 1; // bit 1
        uint8_t is_adjusted        : 1; // bit 2
        uint8_t fault_code         : 4; // bit 3-6
        uint8_t rotation_direction : 1; // bit 7
    }data;
  uint8_t asByte;
};

union CAN_10B_Byte7Struct {
    struct {
        uint8_t unused             : 4; // bit 0-3
        uint8_t angle_source       : 2; // bit 4-5
        uint8_t angle_accurate     : 1; // bit 6
        uint8_t angle_memorized    : 1; // bit 7
    }data;
  uint8_t asByte;
};

// Read left to right in documentation
struct Can10BDashStruct {
    uint8_t SteeringAngle1;
    uint8_t SteeringAngle2;
    uint8_t RotationSpeed;
    CAN_10B_Byte4Struct Status;
    uint8_t CheckSum;
    uint8_t OptmizedRotationSpeed;
    uint8_t Field7;
};
#endif
