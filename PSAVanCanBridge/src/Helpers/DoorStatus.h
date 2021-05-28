// DoorStatus.h
#pragma once

#ifndef _DoorStatus_h
    #define _DoorStatus_h

// Struct to hold the door statuses in one place
typedef struct {
    uint8_t FuelFlap    : 1; // bit 0
    uint8_t Sunroof     : 1; // bit 1
    uint8_t Hood        : 1; // bit 2
    uint8_t BootLid     : 1; // bit 3
    uint8_t RearLeft    : 1; // bit 4
    uint8_t RearRight   : 1; // bit 5
    uint8_t FrontLeft   : 1; // bit 6
    uint8_t FrontRight  : 1; // bit 7
} DoorStatusStruct;


typedef union {
    DoorStatusStruct status;
    uint8_t asByte;
} DoorStatus;

#endif
