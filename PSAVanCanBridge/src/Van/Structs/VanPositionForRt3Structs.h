// VanPositionForRt3Structs.h
#pragma once

#ifndef _VanPositionForRt3Structs_h
    #define _VanPositionForRt3Structs_h

// VANID: 744
const uint16_t VAN_ID_POSITION_FOR_RT3 = 0x744;

const uint8_t VAN_ID_POSITION_FOR_RT3_LENGTH = 12;

//Read right to left in documentation

typedef struct {
    uint16_t wheel_counter       : 15;
    uint16_t error_status        : 1;
} VanWheelCounterAndStatus;

typedef union {
    VanWheelCounterAndStatus status;
    uint16_t asRawValue;
} VanWheelCounterAndStatusUnion;

typedef struct {
    uint8_t rotation_direction           : 1; // bit 0
    uint8_t fault_code                   : 4; // bit 1-4
    uint8_t angle_sensor_adjusted        : 1; // bit 5
    uint8_t angle_sensor_calibrated      : 1; // bit 6
    uint8_t angle_sensor_internal_status : 1; // bit 7
} SteeringWheelStruct;

// Read left to right in documentation
struct VanPositionForRt3Struct {
    uint16_t RearRightWheelSpeed;
    uint16_t RearLeftWheelSpeed;
    VanWheelCounterAndStatusUnion RearRightAbsStatus;
    VanWheelCounterAndStatusUnion RearLeftAbsStatus;
    uint16_t SteeringWheelAnglePosition;
    uint8_t SteeringWheelRotationSpeed;
    SteeringWheelStruct SteeringWheelSensorStatus;
};

union VanPositionForRt3Packet {
    VanPositionForRt3Struct data;
    uint8_t VanPositionForRt3Packet[sizeof(VanPositionForRt3Struct)];
};

#endif
