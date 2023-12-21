// CAN_328.h
#pragma once

#ifndef _CAN_328_h
    #define _CAN_328_h

#include <stdint.h>

const uint16_t CAN_328_INTERVAL = 500;

// CANID: 328
const uint16_t CAN_ID_328 = 0x328;
/*
const uint8_t CAN_RADIO_FREQ_SCAN_STATUS_LISTEN                = 0;
const uint8_t CAN_RADIO_FREQ_SCAN_STATUS_SEARCH_MANUAL         = 1;
const uint8_t CAN_RADIO_FREQ_SCAN_STATUS_SEARCH_AUTO           = 2;
const uint8_t CAN_RADIO_FREQ_SCAN_STATUS_SEARCH_PI             = 3;
const uint8_t CAN_RADIO_FREQ_SCAN_STATUS_SEARCH_PTY            = 4;
const uint8_t CAN_RADIO_FREQ_SCAN_STATUS_SEARCH_EXPLORE_FREQ   = 5;
const uint8_t CAN_RADIO_FREQ_SCAN_STATUS_SEARCH_EXPLORE_MEMORY = 6;
const uint8_t CAN_RADIO_FREQ_SCAN_STATUS_SEARCH_AUTO_MEMORY    = 7;
const uint8_t CAN_RADIO_FREQ_SCAN_STATUS_SEARCH_NO_STATION     = 8;
const uint8_t CAN_RADIO_FREQ_SCAN_STATUS_SEARCH_AUTO_FREQ      = 0xF;

const uint8_t CAN_RADIO_BAND_RESERVED = 0;
const uint8_t CAN_RADIO_BAND_FM1      = 1;
const uint8_t CAN_RADIO_BAND_FM2      = 2;
const uint8_t CAN_RADIO_BAND_FM3      = 3;
const uint8_t CAN_RADIO_BAND_FMT      = 4;
const uint8_t CAN_RADIO_BAND_AM       = 5;
const uint8_t CAN_RADIO_BAND_NONE1    = 6;
const uint8_t CAN_RADIO_BAND_NONE2    = 7;
*/
// Read right to left in documentation
union CAN_328_Byte1Struct {
    struct {
        uint8_t frequency_scan_status    : 4; // bit 0-3
        uint8_t tuner_sensitivity        : 1; // bit 4
        uint8_t frequency_scan_direction : 1; // bit 5
        uint8_t manual_tuner_mode        : 1; // bit 6
        uint8_t list_requested           : 1; // bit 7
    }data;
  uint8_t asByte;
};

// Read right to left in documentation
union CAN_328_Byte2Struct {
    struct {
        uint8_t            : 4; // bit 0-3
        uint8_t memory_num : 4; // bit 4-7
    }data;
  uint8_t asByte;
};

// Read right to left in documentation
union CAN_328_Byte3Struct {
    struct {
        uint8_t           : 4; // bit 0-3
        uint8_t band_type : 3; // bit 4-6
        uint8_t pty_list  : 1; // bit 7
    }data;
  uint8_t asByte;
};

// Read right to left in documentation
union CAN_328_Byte45Struct {
    struct {
        uint16_t frequency : 11; // bit 4.2-5.0
        uint8_t            : 5;  // bit 4.3-4.7
    }data;
  uint16_t asUInt16;
};

// Read left to right in documentation
struct Can328Struct {
    CAN_328_Byte1Struct Field1;
    CAN_328_Byte2Struct BandPosition;
    uint8_t Band;
    uint8_t Frequency1;
    uint8_t Frequency2;
    uint8_t Unknown;
};
#endif
