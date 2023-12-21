// CAN_325.h
#pragma once

#ifndef _CAN_325_h
    #define _CAN_325_h

#include <stdint.h>

const uint16_t CAN_325_INTERVAL = 500;

// CANID: 325
const uint16_t CAN_ID_325 = 0x325;

uint8_t CAN_RADIO_CD_REPETITION_MODE_NORMAL = 0;
uint8_t CAN_RADIO_CD_REPETITION_MODE_TRACK  = 1;
uint8_t CAN_RADIO_CD_REPETITION_MODE_DISC   = 2;

uint8_t CAN_RADIO_CD_PLAY_MODE_NORMAL = 0;
uint8_t CAN_RADIO_CD_PLAY_MODE_RANDOM_PLAY = 1;

uint8_t CAN_RADIO_CD_PLAYBACK_STATUS_NO_DISK = 0;
uint8_t CAN_RADIO_CD_PLAYBACK_STATUS_STOP    = 1;
uint8_t CAN_RADIO_CD_PLAYBACK_STATUS_PAUSE   = 2;
uint8_t CAN_RADIO_CD_PLAYBACK_STATUS_READING = 3;
uint8_t CAN_RADIO_CD_PLAYBACK_STATUS_FAST_FORWARD = 4;
uint8_t CAN_RADIO_CD_PLAYBACK_STATUS_FAST_REVERSE = 5;
uint8_t CAN_RADIO_CD_PLAYBACK_STATUS_INTRO        = 7;


// Read right to left in documentation
union CAN_325_Byte1Struct {
    struct {
        uint8_t play_mode        : 2; // bit 0-1    CAN_RADIO_CD_PLAY_MODE
        uint8_t repeat_mode      : 2; // bit 2-3    CAN_RADIO_CD_REPETITION_MODE
        uint8_t mode_cd_or_track : 1; // bit 5      0: cd 1: track
        uint8_t                  : 2; // bit 5-6
        uint8_t track_list       : 1; // bit 7
    }data;
  uint8_t asByte;
};

union CAN_325_Byte2Struct {
    struct {
        uint8_t cd_playback_status  : 3; // bit 0-1   CAN_RADIO_CD_PLAYBACK_STATUS
        uint8_t cd_text_available   : 1; // bit 2-3
        uint8_t unreadable_disk     : 1; // bit 5
        uint8_t disc_upside_down    : 1; // bit 6
        uint8_t                     : 1; // bit 6
        uint8_t access_cd           : 1; // bit 7
    }data;
  uint8_t asByte;
};

union CAN_325_Byte3Struct {
    struct {
        uint8_t                  : 6; // bit 0-5
        uint8_t error_present    : 1; // bit 6
        uint8_t temperature_high : 1; // bit 7
    }data;
  uint8_t asByte;
};

#endif
