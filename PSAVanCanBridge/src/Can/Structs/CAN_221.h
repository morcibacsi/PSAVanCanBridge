// CanTrip0Structs.h
#pragma once

#ifndef _CanTrip0Structs_h
    #define _CanTrip0Structs_h

// CANID: 221
const uint16_t CAN_ID_TRIP0 = 0x221;

// Read right to left in documentation
union CanTripByte1Struct {
    struct {
        uint8_t left_stick_button         : 1; // bit 0
        uint8_t unused1                   : 1; // bit 1
        uint8_t unused2                   : 1; // bit 2
        uint8_t trip_switch_pressed       : 1; // bit 3
        uint8_t unused4                   : 1; // bit 4
        uint8_t unused5                   : 1; // bit 5
        uint8_t rest_of_run_is_null       : 1; // bit 6
        uint8_t consumption_is_null       : 1; // bit 7
    } data;
    uint8_t asByte;
};

typedef struct {
    unsigned int data : 16;
} CanTripLitersPer100kmStruct;

typedef struct {
    unsigned int data : 16;
} CanRestOfRunOnCurrentFuelLevel;

typedef struct {
    unsigned int data : 16;
} CanRestOfRunToFinish;

//typedef struct {
//    uint8_t bit0 : 1; // bit 0
//    uint8_t bit1 : 1; // bit 1
//    uint8_t bit2 : 1; // bit 2
//    uint8_t bit3 : 1; // bit 3
//    uint8_t bit4 : 1; // bit 4
//    uint8_t bit5 : 1; // bit 5
//    uint8_t bit6 : 1; // bit 6
//    uint8_t economy_mode : 1; // bit 7
//} CanDash1Byte2Struct;


// Read left to right in documentation
struct CanTrip0Struct {
    CanTripByte1Struct Information;
    uint8_t LitersPer100KmByte1;
    uint8_t LitersPer100KmByte2;
    uint8_t KmToGasStationByte1;
    uint8_t KmToGasStationByte2;
    uint8_t KmToFinishByte1;
    uint8_t KmToFinishByte2;
};

union CanTrip0Packet {
    CanTrip0Struct data;
    uint8_t CanTrip0Packet[sizeof(CanTrip0Struct)];
};

#endif
