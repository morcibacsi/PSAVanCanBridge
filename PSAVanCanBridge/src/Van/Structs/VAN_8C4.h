#pragma once

#ifndef _VAN_8C4_h
#define _VAN_8C4_h

#include <stdint.h>

// VANID: 8C4
const uint16_t VAN_ID_EVENT = 0x8C4;

const uint8_t VAN_EVENT_SRC_AC    = 0x0E;
const uint8_t VAN_EVENT_SRC_BSI   = 0x12;
const uint8_t VAN_EVENT_SRC_RADIO = 0x0A;
const uint8_t VAN_EVENT_SRC_NAVI  = 0x07;
const uint8_t VAN_EVENT_SRC_PHONE = 0x1A;
const uint8_t VAN_EVENT_SRC_CDC   = 0x16;
const uint8_t VAN_EVENT_SRC_AAS   = 0x14;

const uint8_t VAN_EVENT_RADIO_BUF_NO_CHANGE    = 0;
const uint8_t VAN_EVENT_RADIO_BUF_TUNER        = 1;
const uint8_t VAN_EVENT_RADIO_BUF_TUNER_REPEAT = 2;
const uint8_t VAN_EVENT_RADIO_BUF_TAPE         = 3;
const uint8_t VAN_EVENT_RADIO_BUF_UNUSED_1     = 4;
const uint8_t VAN_EVENT_RADIO_BUF_UNUSED_2     = 5;
const uint8_t VAN_EVENT_RADIO_BUF_CD_STATE     = 6;
const uint8_t VAN_EVENT_RADIO_BUF_UNUSED_3     = 7;

const uint8_t VAN_EVENT_RADIO_BUTTON_PUSH_TYPE_PUSH       = 0;
const uint8_t VAN_EVENT_RADIO_BUTTON_PUSH_TYPE_RELEASE    = 1;
const uint8_t VAN_EVENT_RADIO_BUTTON_PUSH_TYPE_LONG_PRESS = 2;
const uint8_t VAN_EVENT_RADIO_BUTTON_PUSH_TYPE_REPEAT     = 3;

const uint8_t VAN_EVENT_RADIO_BUTTON_NONE               = 0x00;
const uint8_t VAN_EVENT_RADIO_BUTTON_MEMO1              = 0x01;
const uint8_t VAN_EVENT_RADIO_BUTTON_MEMO2              = 0x02;
const uint8_t VAN_EVENT_RADIO_BUTTON_MEMO3              = 0x03;
const uint8_t VAN_EVENT_RADIO_BUTTON_MEMO4              = 0x04;
const uint8_t VAN_EVENT_RADIO_BUTTON_MEMO5              = 0x05;
const uint8_t VAN_EVENT_RADIO_BUTTON_MEMO6              = 0x06;
const uint8_t VAN_EVENT_RADIO_BUTTON_7                  = 0x07;
const uint8_t VAN_EVENT_RADIO_BUTTON_8                  = 0x08;
const uint8_t VAN_EVENT_RADIO_BUTTON_9                  = 0x09;
const uint8_t VAN_EVENT_RADIO_BUTTON_0                  = 0x0A;
const uint8_t VAN_EVENT_RADIO_BUTTON_HASHTAG            = 0x0B;
const uint8_t VAN_EVENT_RADIO_BUTTON_ASTERISK           = 0x0C;
const uint8_t VAN_EVENT_RADIO_BUTTON_ON_OFF             = 0x10;
const uint8_t VAN_EVENT_RADIO_BUTTON_DOWN_AUDIO_MINUS   = 0x11;
const uint8_t VAN_EVENT_RADIO_BUTTON_UP_AUDIO_PLUS      = 0x12;
const uint8_t VAN_EVENT_RADIO_BUTTON_LEFT_SEARCH_MINUS  = 0x13;
const uint8_t VAN_EVENT_RADIO_BUTTON_RIGHT_SEARCH_PLUS  = 0x14;
const uint8_t VAN_EVENT_RADIO_BUTTON_VALIDATE           = 0x15;
const uint8_t VAN_EVENT_RADIO_BUTTON_AUDIO              = 0x16;
const uint8_t VAN_EVENT_RADIO_BUTTON_MANUAL_AUTO        = 0x17;
const uint8_t VAN_EVENT_RADIO_BUTTON_SOURCE             = 0x1A;
const uint8_t VAN_EVENT_RADIO_BUTTON_TUNER              = 0x1B;
const uint8_t VAN_EVENT_RADIO_BUTTON_TAPE               = 0x1C;
const uint8_t VAN_EVENT_RADIO_BUTTON_CD_MD              = 0x1D;
const uint8_t VAN_EVENT_RADIO_BUTTON_CDC                = 0x1E;
const uint8_t VAN_EVENT_RADIO_BUTTON_RESERVED_PHONE     = 0x1F;
const uint8_t VAN_EVENT_RADIO_BUTTON_RDS                = 0x20;
const uint8_t VAN_EVENT_RADIO_BUTTON_TA                 = 0x21;
const uint8_t VAN_EVENT_RADIO_BUTTON_PTY                = 0x22;
const uint8_t VAN_EVENT_RADIO_BUTTON_BND                = 0x23;
const uint8_t VAN_EVENT_RADIO_BUTTON_FM                 = 0x24;
const uint8_t VAN_EVENT_RADIO_BUTTON_AM                 = 0x25;
const uint8_t VAN_EVENT_RADIO_BUTTON_RESERVED_TAPE      = 0x2A;
const uint8_t VAN_EVENT_RADIO_BUTTON_RESERVED_CD        = 0x30;
const uint8_t VAN_EVENT_RADIO_BUTTON_PHONE_DECR         = 0x3A;
const uint8_t VAN_EVENT_RADIO_BUTTON_PHONE_RACCR        = 0x3B;
const uint8_t VAN_EVENT_RADIO_BUTTON_PHONE_SUPPR        = 0x3C;
const uint8_t VAN_EVENT_RADIO_BUTTON_PHONE_SOS          = 0x3D;

// Read right to left in documentation
union VanEventByte1Struct {
    struct {
        uint8_t event_src     : 5; // bit 0-4       VAN_EVENT_SRC
        uint8_t               : 1; // bit 5
        uint8_t no_action     : 1; // bit 6
        uint8_t primary_event : 1; // bit 7
    } data;
    uint8_t asByte;
};

union VanEventRadioByte2Struct {
    struct {
        uint8_t event_general  : 1; // bit 0
        uint8_t event_keyboard : 1; // bit 1
        uint8_t event_radio    : 1; // bit 2
        uint8_t event_tape     : 1; // bit 3
        uint8_t event_cd       : 1; // bit 4
        uint8_t buffer_part    : 3; // bit 5-7      VAN_EVENT_RADIO_BUF
    } data;
    uint8_t asByte;
};

union VanEventRadioByte3Struct {
    struct {
        uint8_t radio_button : 6; // bit 0-5        VAN_EVENT_RADIO_BUTTON
        uint8_t push_type    : 2; // bit 6-7        VAN_EVENT_RADIO_BUTTON_PUSH_TYPE
    } data;
    uint8_t asByte;
};

//Read left to right in documentation
struct VanEventRadioStructs {
    VanEventByte1Struct EventId;
    VanEventRadioByte2Struct Event;
    VanEventRadioByte3Struct Button;
};

//----------------------------------------------------------------------------------------//

union VanEventBSIByte2Struct {
    struct {
        uint8_t parking_aid_beep_request      : 1; // bit 0
        uint8_t parking_aid_display_request   : 1; // bit 1
        uint8_t                               : 1; // bit 2
        uint8_t trip_change_status            : 1; // bit 3
        uint8_t                               : 1; // bit 4
        uint8_t door_change_status            : 1; // bit 5
        uint8_t signal_and_mode_change_status : 1; // bit 6
        uint8_t alert_change_status           : 1; // bit 7
    } data;
    uint8_t asByte;
};

struct VanEventBSIStructs {
    VanEventByte1Struct EventId;
    VanEventBSIByte2Struct Event;
};

//----------------------------------------------------------------------------------------//
#endif