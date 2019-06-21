// CanDash2Structs.h
#pragma once

#ifndef _CanDash2Structs_h
    #define _CanDash2Structs_h

// CANID: 128
const unsigned long CAN_ID_DASH2 = 0x128;

// Read right to left in documentation
typedef struct {
    uint8_t                       : 1; // bit 0
    uint8_t                       : 1; // bit 1
    uint8_t                       : 1; // bit 2
    uint8_t                       : 1; // bit 3
    uint8_t                       : 1; // bit 4
    uint8_t                       : 1; // bit 5
    uint8_t driver_seatbelt_light : 1; // bit 6
    uint8_t                       : 1; // bit 7
} CanDash2Byte1Struct;

typedef struct {
    uint8_t               : 1; // bit 0
    uint8_t               : 1; // bit 1
    uint8_t               : 1; // bit 2
    uint8_t               : 1; // bit 3
    uint8_t any_door_open : 1; // bit 4
    uint8_t               : 1; // bit 5
    uint8_t               : 1; // bit 6
    uint8_t               : 1; // bit 7
} CanDash2Byte2Struct;

typedef struct {
    uint8_t                    : 1; // bit 0
    uint8_t left_indicator_on  : 1; // bit 1
    uint8_t right_indicator_on : 1; // bit 2
    uint8_t rear_fog_light_on  : 1; // bit 3
    uint8_t front_fog_light_on : 1; // bit 4
    uint8_t high_beam_on       : 1; // bit 5
    uint8_t low_beam_on        : 1; // bit 6
    uint8_t side_lights_on     : 1; // bit 7
} CanDash2Byte5Struct;

typedef struct {
    uint8_t                : 1; // bit 0
    uint8_t                : 1; // bit 1
    uint8_t                : 1; // bit 2
    uint8_t                : 1; // bit 3
    uint8_t                : 1; // bit 4
    uint8_t                : 1; // bit 5
    uint8_t                : 1; // bit 6
    uint8_t low_fuel_level : 1; // bit 7
} CanDash2Byte6Struct;


// Read left to right in documentation
typedef struct CanDash2Struct {
    CanDash2Byte1Struct Field1;
    CanDash2Byte2Struct Field2;
    uint8_t Field3;
    uint8_t Field4;
    CanDash2Byte5Struct Field5;
    CanDash2Byte6Struct Field6;
    uint8_t Field7;
    uint8_t Field8;
};

typedef union CanDash2Packet {
    CanDash2Struct data;
    uint8_t CanDash2Packet[sizeof(CanDash2Struct)];
};

#endif
