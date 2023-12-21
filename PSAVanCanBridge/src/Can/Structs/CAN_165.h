// CAN_165.h
#pragma once

#ifndef _CAN_165_h
    #define _CAN_165_h

#include <stdint.h>

const uint16_t CAN_165_INTERVAL = 500;

// CANID: 165
const uint16_t CAN_ID_165 = 0x165;

const uint8_t CAN_RADIO_HAS_CD_NO = 0;
const uint8_t CAN_RADIO_HAS_CD_YES_FAILURE = 1;
const uint8_t CAN_RADIO_HAS_CD_YES_OK = 2;

const uint8_t CAN_RADIO_HAS_CDC_NO = 0;
const uint8_t CAN_RADIO_HAS_CDC_YES_FAILURE = 1;
const uint8_t CAN_RADIO_HAS_CDC_YES_OK = 2;

const uint8_t CAN_RADIO_SRC_NONE = 0;
const uint8_t CAN_RADIO_SRC_TUNER = 1;
const uint8_t CAN_RADIO_SRC_CD = 2;
const uint8_t CAN_RADIO_SRC_CDC = 3;
const uint8_t CAN_RADIO_SRC_AUX1 = 4;
const uint8_t CAN_RADIO_SRC_AUX2 = 5;

// Read right to left in documentation
union CAN_165_Byte1Struct {
    struct {
        uint8_t              : 2; // bit 0-1
        uint8_t hifi         : 1; // bit 2
        uint8_t bi_tuner     : 1; // bit 3
        uint8_t on_refused   : 1; // bit 4
        uint8_t muted        : 1; // bit 5
        uint8_t amplifier_on : 1; // bit 6
        uint8_t power        : 1; // bit 7
    }data;
  uint8_t asByte;
};

union CAN_165_Byte2Struct {
    struct {
        uint8_t           : 3; // bit 0-2
        uint8_t has_cdc   : 2; // bit 3-4
        uint8_t has_cd    : 2; // bit 5-6
        uint8_t has_tuner : 1; // bit 7
    }data;
  uint8_t asByte;
};

union CAN_165_Byte3Struct {
    struct {
        uint8_t source_refused : 3; // bit 0-2
        uint8_t                : 1; // bit 3
        uint8_t source         : 3; // bit 4-6
        uint8_t                : 1; // bit 7
    }data;
  uint8_t asByte;
};

union CAN_165_Byte4Struct {
    struct {
        uint8_t                       : 2; // bit 0-1
        uint8_t digital_radio_support : 3; // bit 2-4
        uint8_t cd_format_support     : 3; // bit 5-7
    }data;
  uint8_t asByte;
};

// Read left to right in documentation
struct Can165Struct {
    CAN_165_Byte1Struct General1;
    CAN_165_Byte2Struct General2;
    CAN_165_Byte3Struct General3;
    uint8_t Status;
    uint8_t CheckSum;
    uint8_t Counter;
};
#endif
