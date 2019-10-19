// CanAirConStructs.h
#pragma once

#ifndef _CanAirConStructs_h
    #define _CanAirConStructs_h

// CANID: 1D0
const uint16_t CAN_ID_AIRCON = 0x1D0;

const uint8_t CAN_AIRCON_DIR_FRONT          = B011;
const uint8_t CAN_AIRCON_DIR_UP             = B100;
const uint8_t CAN_AIRCON_DIR_UP_AND_DOWN    = B110;
const uint8_t CAN_AIRCON_DIR_DOWN           = B010;
const uint8_t CAN_AIRCON_DIR_FRONT_AND_DOWN = B101;
const uint8_t CAN_AIRCON_DIR_FRONT_UP2      = B001;

// Read right to left in documentation
typedef struct {
    uint8_t fan_speed : 3; // bit 0-2
    uint8_t           : 1; // bit 3
    uint8_t           : 1; // bit 4-6
    uint8_t           : 1; // bit 7
} CanAirConByte3Struct;

typedef struct {
    uint8_t                 : 1; // bit 0
    uint8_t                 : 1; // bit 1
    uint8_t                 : 1; // bit 2
    uint8_t                 : 1; // bit 3
    uint8_t air_direction   : 1; // bit 4-6
    uint8_t                 : 1; // bit 7
} CanAirConByte4Struct;

typedef struct {
    uint8_t                 : 1; // bit 0
    uint8_t                 : 1; // bit 1
    uint8_t                 : 1; // bit 2
    uint8_t                 : 1; // bit 3
    uint8_t windshield_blow : 1; // bit 4
    uint8_t air_recycling   : 1; // bit 5
    uint8_t                 : 1; // bit 6
    uint8_t                 : 1; // bit 7
} CanAirConByte5Struct;

typedef struct {
    uint8_t left_zone_temperature : 4; // bit 0-3
    uint8_t                       : 1; // bit 4
    uint8_t                       : 1; // bit 5
    uint8_t                       : 1; // bit 6
    uint8_t                       : 1; // bit 7
} CanAirConByte6Struct;

typedef struct {
    uint8_t right_zone_temperature : 4; // bit 0-3
    uint8_t                        : 1; // bit 4
    uint8_t                        : 1; // bit 5
    uint8_t                        : 1; // bit 6
    uint8_t                        : 1; // bit 7
} CanAirConByte7Struct;

// Read left to right in documentation
typedef struct CanAirConStruct {
    uint8_t Field1;
    uint8_t Field2;
    CanAirConByte3Struct FanSpeed;
    CanAirConByte4Struct Direction;
    CanAirConByte5Struct Field5;
    CanAirConByte6Struct TemperatureLeft;
    CanAirConByte7Struct TemperatureRight;
};

typedef union CanAirConPacket {
    CanAirConStruct data;
    uint8_t CanAirConPacket[sizeof(CanAirConStruct)];
};

#endif
