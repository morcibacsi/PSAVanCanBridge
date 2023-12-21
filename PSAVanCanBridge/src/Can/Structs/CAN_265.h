// CAN_265.h
#pragma once

#ifndef _CAN_265_h
    #define _CAN_265_h

#include <stdint.h>

const uint16_t CAN_265_INTERVAL = 500;

// CANID: 265
const uint16_t CAN_ID_265 = 0x265;

const uint8_t CAN_RADIO_PTY_MODE_NOT_REQUESTED = 0;
const uint8_t CAN_RADIO_PTY_MODE_IN_SELECTION  = 1;
const uint8_t CAN_RADIO_PTY_MODE_IN_LISTENING  = 2;

const uint8_t CAN_RADIO_PTY_LISTEN_NOT_AVAILABLE = 0;
const uint8_t CAN_RADIO_PTY_LISTEN_DIFFERENT_THAN_SELECTED = 1;
const uint8_t CAN_RADIO_PTY_LISTEN_SAME_AS_SELECTED = 2;
const uint8_t CAN_RADIO_PTY_LISTEN_RDS_IN_PROGRESS = 3;

const uint8_t CAN_RADIO_TMC_NOT_AVAILABLE = 0;
const uint8_t CAN_RADIO_TMC_AVAILABLE = 1;
const uint8_t CAN_RADIO_TMC_RDS_IN_PROGRESS = 3;

// Read right to left in documentation
union CAN_265_Byte1Struct {
    struct {
        uint8_t reg            : 1; // bit 0
        uint8_t ta_in_progress : 1; // bit 1
        uint8_t ta_ok          : 2; // bit 2-3
        uint8_t ta             : 1; // bit 4
        uint8_t af_ok          : 2; // bit 5-6
        uint8_t af             : 1; // bit 7
    }data;
  uint8_t asByte;
};

union CAN_265_Byte2Struct {
    struct {
        uint8_t                      : 1; // bit 0
        uint8_t tmc_available        : 2; // bit 1-2 CAN_RADIO_TMC
        uint8_t tmc_requested        : 1; // bit 3
        uint8_t pty_mode_in_progress : 2; // bit 4-5 CAN_RADIO_PTY_LISTEN
        uint8_t pty_mode_request     : 2; // bit 6-7 CAN_RADIO_PTY_MODE
    }data;
  uint8_t asByte;
};

#endif
