#pragma once

#ifndef _VAN_4EC_h
    #define _VAN_4EC_h

#include <stdint.h>

const uint8_t VAN_ID_CDC_LENGTH = 12;

// VANID: 4EC
const uint16_t VAN_ID_CDC = 0x4EC;

const uint8_t VAN_CDC_REPEAT_MODE_NORMAL = 0;
const uint8_t VAN_CDC_REPEAT_MODE_TRACK  = 1;
const uint8_t VAN_CDC_REPEAT_MODE_DISK   = 2;

const uint8_t VAN_CDC_READ_MODE_NORMAL                = 0;
const uint8_t VAN_CDC_READ_MODE_RANDOM_TRACK          = 1;
const uint8_t VAN_CDC_READ_MODE_RANDOM_DISK           = 2;
const uint8_t VAN_CDC_READ_MODE_RANDOM_DISK_AND_TRACK = 3;

const uint8_t VAN_CDC_PLAYBACK_STATUS_NO_DISK = 0;
const uint8_t VAN_CDC_PLAYBACK_STATUS_STOP    = 1;
const uint8_t VAN_CDC_PLAYBACK_STATUS_PAUSE   = 2;
const uint8_t VAN_CDC_PLAYBACK_STATUS_READING = 3;
const uint8_t VAN_CDC_PLAYBACK_STATUS_FAST_FORWARD = 4;
const uint8_t VAN_CDC_PLAYBACK_STATUS_FAST_REVERSE = 5;
const uint8_t VAN_CDC_PLAYBACK_STATUS_INTRO        = 7;

// Read right to left in documentation
union VanCdcByte2Struct {
    struct {
        uint8_t read_mode   : 2; // bit 0-1     VAN_CDC_READ_MODE
        uint8_t repeat_mode : 2; // bit 2-3     VAN_CDC_REPEAT_MODE
        uint8_t             : 4; // bit 4-7
    } data;
    uint8_t asByte;
};

union VanCdcByte3Struct {
    struct {
        uint8_t playback_status       : 3; // bit 0-2    VAN_CDC_PLAYBACK_STATUS
        uint8_t check_slots           : 1; // bit 3
        uint8_t access_disk           : 1; // bit 4
        uint8_t                       : 1; // bit 5
        uint8_t van_network_available : 1; // bit 6
        uint8_t power_on              : 1; // bit 7
    } data;
    uint8_t asByte;
};

union VanCdcByte4Struct {
    struct {
        uint8_t slot_capacity     : 4; // bit 0-3
        uint8_t slots_available   : 1; // bit 4
        uint8_t door_open         : 1; // bit 5
        uint8_t cdc_error         : 1; // bit 6
        uint8_t tempertature_high : 1; // bit 7
    } data;
    uint8_t asByte;
};

union VanCdcByte11Struct {
    struct {
        uint8_t disk_1_present : 1; // bit 0
        uint8_t disk_2_present : 1; // bit 1
        uint8_t disk_3_present : 1; // bit 2
        uint8_t disk_4_present : 1; // bit 3
        uint8_t disk_5_present : 1; // bit 4
        uint8_t disk_6_present : 1; // bit 5
        uint8_t disk_7_present : 1; // bit 6
        uint8_t disk_8_present : 1; // bit 7
    } data;
    uint8_t asByte;
};

// Read left to right in documentation
struct VanCdcStruct {
    uint8_t Header;
    VanCdcByte2Struct Mode;
    VanCdcByte3Struct Status1;
    VanCdcByte4Struct Status2;
    uint8_t CurrentMinutes;
    uint8_t CurrentSeconds;
    uint8_t CurrentTrack;
    uint8_t CurrentDisk;
    uint8_t TrackCount;
    uint8_t Unused;
    VanCdcByte11Struct DisksPresent;
    uint8_t Footer;
};

#endif
