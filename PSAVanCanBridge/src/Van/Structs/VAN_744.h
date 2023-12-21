#pragma once

#ifndef _VAN_744_h
#define _VAN_744_h

#include <stdint.h>

// VANID: 8A4
const uint16_t VAN_ID_744 = 0x744;

const uint8_t VAN_ROTATE_CLOCKWISE = 0;
const uint8_t VAN_ROTATE_COUNTER_CLOCKWISE = 1;

// Read right to left in documentation
union VAN_744_Byte12Struct {
    struct {
        uint8_t intenal_status     : 1; // bit 0
        uint8_t is_calibrated      : 1; // bit 1
        uint8_t is_adjusted        : 1; // bit 2
        uint8_t fault_code         : 4; // bit 3-6
        uint8_t rotation_direction : 1; // bit 7
    }data;
  uint8_t asByte;
};

//Read left to right in documentation
struct VanRearWheelDataStructs {
    uint8_t RearRightWheelSpeed1;
    uint8_t RearRightWheelSpeed2;
    uint8_t RearLeftWheelSpeed1;
    uint8_t RearLeftWheelSpeed2;

    uint8_t RearRightWheelCounter1;
    uint8_t RearRightWheelCounter2;
    uint8_t RearLeftWheelCounter1;
    uint8_t RearLeftWheelCounter2;

    uint8_t SteeringAngle1;
    uint8_t SteeringAngle2;

    uint8_t SteeringSpeed;
    VAN_744_Byte12Struct SteeringStatus;
};
#endif