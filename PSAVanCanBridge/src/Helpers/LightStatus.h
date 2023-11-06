// LightStatus.h
#pragma once

#ifndef _LightStatus_h
    #define _LightStatus_h

// Struct to hold the light statuses in one place
typedef struct {
    uint8_t LowBeam        : 1; // bit 0 
    uint8_t HighBeam       : 1; // bit 1
    uint8_t FrontFog       : 1; // bit 2
    uint8_t RearFog        : 1; // bit 3
    uint8_t LeftIndicator  : 1; // bit 4
    uint8_t RightIndicator : 1; // bit 5
    uint8_t SideLights     : 1; // bit 6
    uint8_t                : 1; // bit 7
} LightStatusStruct;


typedef union {
    LightStatusStruct status;
    uint8_t asByte;
} LightStatus;

#endif
