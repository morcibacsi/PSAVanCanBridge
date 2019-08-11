// DashIcons1.h
#pragma once

#ifndef _DashIcons1_h
    #define _DashIcons1_h

// Struct to hold the light statuses in one place
typedef struct {
    uint8_t SeatBeltWarning : 1; // bit 0 
    uint8_t FuelLowLight    : 1; // bit 1
    uint8_t PassengerAirbag : 1; // bit 2
    uint8_t Handbrake       : 1; // bit 3
    uint8_t Abs             : 1; // bit 4
    uint8_t Esp             : 1; // bit 5
    uint8_t Mil             : 1; // bit 6
    uint8_t Airbag          : 1; // bit 7
} DashIcons1Struct;


typedef union {
    DashIcons1Struct status;
    uint8_t asByte;
} DashIcons1;

#endif
