#pragma once

#ifndef _CAN_350_2010_h
    #define _CAN_350_2010_h

#include <stdint.h>

// CANID: 350
const uint16_t CAN_350_2010 = 0x350;

// Read right to left in documentation
union CAN_350_2010Byte1Struct {
    struct {
        uint8_t ac_type                : 2; // bit 0-1
        uint8_t ac_request             : 2; // bit 2-3
        uint8_t visibility_request     : 1; // bit 4
        uint8_t rear_window_heating_on : 1; // bit 5
        uint8_t rest_function_status   : 1; // bit 6
        uint8_t reserved               : 1; // bit 7
    } data;
    uint8_t asByte;
};

union CAN_350_2010Byte4Struct {
    struct {
        uint8_t front_left_temperature : 5; // bit 0-4
        uint8_t reserved               : 3; // bit 5-7
    } data;
    uint8_t asByte;
};

union CAN_350_2010Byte5Struct {
    struct {
        uint8_t front_right_temperature : 5; // bit 0-4
        uint8_t reserved                : 3; // bit 5-7
    } data;
    uint8_t asByte;
};

union CAN_350_2010Byte6Struct {
    struct {
        uint8_t fan_speed : 4; // bit 4-6
        uint8_t air_inlet : 3; // bit 4-6
        uint8_t reserved  : 1; // bit 5-7
    } data;
    uint8_t asByte;
};

union CAN_350_2010Byte7Struct {
    struct {
        uint8_t front_right_distribution  : 4; // bit 0-3
        uint8_t front_left_distribution   : 4; // bit 4-7
    } data;
    uint8_t asByte;
};

// Read left to right in documentation
struct CAN_350_2010Struct {
    CAN_350_2010Byte1Struct Modes;
    uint8_t Unused2;
    uint8_t Unused3;
    CAN_350_2010Byte4Struct TemperatureLeft;
    CAN_350_2010Byte5Struct TemperatureRight;
    CAN_350_2010Byte6Struct FanSpeed;
    CAN_350_2010Byte7Struct AirDirection;
    uint8_t CabinPollutionLevel;
};

union CAN_350_2010Packet {
    CAN_350_2010Struct data;
    uint8_t CAN_350_2010Packet[sizeof(CAN_350_2010Struct)];
};

uint8_t static GetFanSpeed_2010(uint8_t numberFrom0To8)
{
    uint8_t result = 0;
    if (0 <= numberFrom0To8 && numberFrom0To8 <= 8)
    {
        if (numberFrom0To8 == 0)
        {
            result = 1;
        }
        else
        {
            result = numberFrom0To8 + 1;
        }
    }
    else {
        if (numberFrom0To8 > 8)
        {
            result = 9;
        }
        if (numberFrom0To8 < 0)
        {
            result = 1;
        }
    }

    return result;
}
#endif
