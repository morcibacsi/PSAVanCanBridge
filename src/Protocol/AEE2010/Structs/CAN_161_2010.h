// CAN_161_2010.h
#pragma once

#ifndef _CAN_161_2010_h
    #define _CAN_161_2010_h

#include <stdint.h>

const uint16_t CAN_161_2010_INTERVAL = 500;

// CANID: 161
const uint16_t CAN_ID_161_2010 = 0x161;

// Read right to left in documentation
union CAN_161_2010_Byte5Struct {
    struct {
        uint8_t : 1; // bit 0
        uint8_t fuel_tank_capacity_in_liters : 7; // bit 1-7
    } data;
    uint8_t asByte;
};

#endif
