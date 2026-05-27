// CAN_1E9_2010.h
#pragma once

#ifndef _CAN_1E9_2010_h
    #define _CAN_1E9_2010_h

#include <stdint.h>

// CANID: 1E9
const uint16_t CAN_ID_1E9_2010 = 0x1E9;
const uint8_t CAN_ID_1E9_2010_LEN = 6;

// Read right to left in documentation
union CAN_1E9_2010_Byte3Struct {
    struct {
        uint8_t unknown_0_2                 : 3; // bit 0-2
        uint8_t speed_display_authorization : 1; // bit 3
        uint8_t radar_poi_pictogram         : 4; // bit 4-7
    } data;
    uint8_t asByte;
};

union CAN_1E9_2010_Byte4Struct {
    struct {
        uint8_t unknown_0_1                 : 2; // bit 0-1
        uint8_t radar_poi_pictogram2        : 6; // bit 2-7
    } data;
    uint8_t asByte;
};

// Read left to right in documentation
struct CAN_1E9_2010_Struct {
    uint8_t SpeedLimit;
    uint8_t AdvisedSpeed;
    CAN_1E9_2010_Byte3Struct SpeedLimitStatus;
    CAN_1E9_2010_Byte4Struct RadarPoiPictogram2;
    uint8_t DurationOnNextIncident;
    uint8_t DistanceOfNextIncident;
};

#endif
