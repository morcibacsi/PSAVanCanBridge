// CAN_1E5.h
#pragma once

#ifndef _CAN_1E5_h
    #define _CAN_1E5_h

#include <stdint.h>

const uint16_t CAN_1E5_INTERVAL = 500;

// CANID: 1E5
const uint16_t CAN_ID_1E5 = 0x1E5;

const uint8_t CAN_AUDIO_SETTINGS_AUTO_VOL_OFF  = 0;
const uint8_t CAN_AUDIO_SETTINGS_AUTO_VOL_ON   = 7;

// Read right to left in documentation
union CAN_1E5_Byte6Struct {
    struct {
        uint8_t auto_vol      : 3; // bit 0-2
        uint8_t               : 1; // bit 3
        uint8_t show_auto_vol : 1; // bit 4
        uint8_t               : 1; // bit 5
        uint8_t loudness_on   : 1; // bit 6
        uint8_t show_loudness : 1; // bit 7
    }data;
  uint8_t asByte;
};

#endif
