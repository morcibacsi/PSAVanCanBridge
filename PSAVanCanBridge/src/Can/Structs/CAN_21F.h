// CAN_21F.h
#pragma once

#ifndef _CAN_21F_h
    #define _CAN_21F_h

#include <stdint.h>

const uint16_t CAN_21F_INTERVAL = 100;

// CANID: 21F
const uint16_t CAN_ID_RADIO_REMOTE = 0x21F;

// Read right to left in documentation
union CAN_21F_Byte1Struct {
    struct {
        uint8_t                        : 1; // bit 0
        uint8_t mode_phone             : 1; // bit 1
        uint8_t volume_minus           : 1; // bit 2
        uint8_t volume_plus            : 1; // bit 3
        uint8_t owerflow_scan_negative : 1; // bit 4
        uint8_t owerflow_scan_positive : 1; // bit 5
        uint8_t seek_down              : 1; // bit 6
        uint8_t seek_up                : 1; // bit 7
    }data;
  uint8_t asByte;
};

// Read left to right in documentation
struct CanDash1Struct {
    CAN_21F_Byte1Struct Command;
    uint8_t ScrollPosition;
};
#endif
