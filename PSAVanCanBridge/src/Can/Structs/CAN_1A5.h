// CAN_1A5.h
#pragma once

#ifndef _CAN_1A5_h
    #define _CAN_1A5_h

#include <stdint.h>

const uint16_t CAN_1A5_INTERVAL = 500;

// CANID: 1A5
const uint16_t CAN_ID_1A5 = 0x1A5;

const uint8_t CAN_RADIO_VOLUME_CHANGE_USER   = 0;
const uint8_t CAN_RADIO_VOLUME_CHANGE_SOURCE = 1;
const uint8_t CAN_RADIO_VOLUME_CHANGE_SPEED_CONTROL = 2;
const uint8_t CAN_RADIO_VOLUME_CHANGE_PARKING_ASSISTANCE = 3;
const uint8_t CAN_RADIO_VOLUME_CHANGE_THERMAL_PROTECTION = 4;
const uint8_t CAN_RADIO_VOLUME_CHANGE_OVERRUN = 5;
const uint8_t CAN_RADIO_VOLUME_CHANGE_OTHER = 7;

// Read right to left in documentation
union CAN_1A5_Byte1Struct {
    struct {
        uint8_t level        : 5; // bit 0-4
        uint8_t origin       : 3; // bit 5-7
    }data;
  uint8_t asByte;
};

// Read left to right in documentation
struct Can1A5Struct {
    CAN_1A5_Byte1Struct Value;
};
#endif
