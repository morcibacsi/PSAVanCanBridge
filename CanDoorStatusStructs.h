// CanDoorStatusStructs.h
#pragma once

#ifndef _CanDoorStatusStructs_h
    #define _CanDoorStatusStructs_h

// CANID: 220
const unsigned long CAN_ID_DOOR_STATUS = 0x220;

// Read right to left in documentation
///*
typedef struct {
    uint8_t             : 1; // bit 0
    uint8_t             : 1; // bit 1
    uint8_t             : 1; // bit 2
    uint8_t trunk       : 1; // bit 3
    uint8_t rear_right  : 1; // bit 4
    uint8_t rear_left   : 1; // bit 5
    uint8_t front_right : 1; // bit 6
    uint8_t front_left  : 1; // bit 7
} CanDoorStatusByte1Struct;

typedef struct {
    uint8_t bit0 : 1; // bit 0
    uint8_t bit1 : 1; // bit 1
    uint8_t bit2 : 1; // bit 2
    uint8_t bit3 : 1; // bit 3
    uint8_t bit4 : 1; // bit 4
    uint8_t bit5 : 1; // bit 5
    uint8_t bit6 : 1; // bit 6
    uint8_t economy_mode : 1; // bit 7
} CanDoorStatusByte2Struct;

// Read left to right in documentation
typedef struct CanDoorStatusStruct {
    CanDoorStatusByte1Struct DoorStatuses;
    CanDoorStatusByte2Struct Field2;
};

typedef union CanDoorStatusPacket {
    CanDoorStatusStruct data;
    byte CanDoorStatusPacket[sizeof(CanDoorStatusStruct)];
};
//*/
#endif
