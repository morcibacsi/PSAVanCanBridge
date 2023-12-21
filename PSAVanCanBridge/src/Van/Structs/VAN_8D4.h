// VanAirConditioner1Structs.h
#pragma once

#ifndef _VAN_8D4_h
    #define _VAN_8D4_h

#include <stdint.h>

const uint8_t VAN_ID_RADIO_COMMAND_GEN_LENGTH = 2;
const uint8_t VAN_ID_RADIO_COMMAND_SRC_LENGTH = 2;

// VANID: 8D4
const uint16_t VAN_ID_RADIO_COMMAND_GEN = 0x8D4;

const uint8_t VAN_RADIO_COMMAND_GEN_CMD_ID_GEN                     = 0x11;
const uint8_t VAN_RADIO_COMMAND_GEN_CMD_ID_SRC                     = 0x12;
const uint8_t VAN_RADIO_COMMAND_GEN_CMD_ID_VOL                     = 0x13;
const uint8_t VAN_RADIO_COMMAND_GEN_CMD_ID_AUD                     = 0x14;
const uint8_t VAN_RADIO_COMMAND_GEN_CMD_ID_TUNER_MEM               = 0x21;
const uint8_t VAN_RADIO_COMMAND_GEN_CMD_ID_TUNER_TUN               = 0x22;
const uint8_t VAN_RADIO_COMMAND_GEN_CMD_ID_TUNER_FRQ               = 0x23;
const uint8_t VAN_RADIO_COMMAND_GEN_CMD_ID_TUNER_RDS               = 0x24;
const uint8_t VAN_RADIO_COMMAND_GEN_CMD_ID_TUNER_PI                = 0x25;
const uint8_t VAN_RADIO_COMMAND_GEN_CMD_ID_TUNER_PTY_SEL           = 0x26;
const uint8_t VAN_RADIO_COMMAND_GEN_CMD_ID_TUNER_REPERTOIRE        = 0x27;
const uint8_t VAN_RADIO_COMMAND_GEN_CMD_ID_TAPE1                   = 0x31;
const uint8_t VAN_RADIO_COMMAND_GEN_CMD_ID_TAPE2                   = 0x32;
const uint8_t VAN_RADIO_COMMAND_GEN_CMD_ID_CD                      = 0x61;
const uint8_t VAN_RADIO_COMMAND_GEN_CMD_ID_AAS                     = 0x81;
const uint8_t VAN_RADIO_COMMAND_GEN_CMD_ID_TUNER_STATUS            = 0xD1;
const uint8_t VAN_RADIO_COMMAND_GEN_CMD_ID_TAPE_STATUS             = 0xD2;
const uint8_t VAN_RADIO_COMMAND_GEN_CMD_ID_TUNER_REPERTOIRE_STATUS = 0xD3;
const uint8_t VAN_RADIO_COMMAND_GEN_CMD_ID_EG_DSP_STATUS           = 0xD4;
const uint8_t VAN_RADIO_COMMAND_GEN_CMD_ID_DCC_MD_STATUS           = 0xD5;
const uint8_t VAN_RADIO_COMMAND_GEN_CMD_ID_CD_STATUS               = 0xD6;

// Read right to left in documentation
union VAN_RAD_CMD_GEN_Byte1Struct {
    struct {
        uint8_t mute           : 1; // bit 0
        uint8_t auto_volume_on : 1; // bit 1
        uint8_t generate_beep  : 2; // bit 2-3
        uint8_t loudness_on    : 1; // bit 4
        uint8_t button_mode    : 1; // bit 5 0: normal 1: keypad
        uint8_t active_on      : 1; // bit 6
        uint8_t power_on       : 1; // bit 7
    } data;
    uint8_t asByte;
};

// Read left to right in documentation
struct VAN_RAD_CMD_GEN_Struct {
    uint8_t CommandId;
    VAN_RAD_CMD_GEN_Byte1Struct Status;
};

union VAN_RAD_CMD_GEN_Packet {
    VAN_RAD_CMD_GEN_Struct data;
    uint8_t VAN_RAD_CMD_GEN_Packet[sizeof(VAN_RAD_CMD_GEN_Struct)];
};

//----------------------------------------------------------------------------------------//

const uint8_t VAN_RAD_CMD_SRC_SRC_NONE            = 0;
const uint8_t VAN_RAD_CMD_SRC_SRC_TUNER           = 1;
const uint8_t VAN_RAD_CMD_SRC_SRC_K7_MD_CD        = 2;
const uint8_t VAN_RAD_CMD_SRC_SRC_CDC             = 3;
const uint8_t VAN_RAD_CMD_SRC_SRC_PHONE           = 4;
const uint8_t VAN_RAD_CMD_SRC_SRC_VOICE_SYNTHESIS = 5;
const uint8_t VAN_RAD_CMD_SRC_SRC_RESERVED        = 6;
const uint8_t VAN_RAD_CMD_SRC_SRC_TV              = 7;
const uint8_t VAN_RAD_CMD_SRC_SRC_AUX             = 8;

// Read right to left in documentation
union VAN_RAD_CMD_SRC_Byte1Struct {
    struct {
        uint8_t source : 4; // bit 0-3
        uint8_t        : 4; // bit 4-7
    } data;
    uint8_t asByte;
};

// Read left to right in documentation
struct VAN_RAD_CMD_SRC_Struct {
    uint8_t CommandId;
    VAN_RAD_CMD_SRC_Byte1Struct Source;
};

//----------------------------------------------------------------------------------------//

union VAN_RAD_CMD_AUD_Byte2Struct {
    struct {
        uint8_t balance_value           : 7; // bit 0-6
        uint8_t exit_from_settings_mode : 1; // bit 7
    } data;
    uint8_t asByte;
};

union VAN_RAD_CMD_AUD_ByteStruct {
    struct {
        uint8_t value : 7; // bit 0-6
        uint8_t       : 1; // bit 7
    } data;
    uint8_t asByte;
};

struct VAN_RAD_CMD_AUD_Struct {
    uint8_t CommandId;
    VAN_RAD_CMD_AUD_Byte2Struct Balance;
    VAN_RAD_CMD_AUD_ByteStruct  Fader;
    VAN_RAD_CMD_AUD_ByteStruct  Bass;
    VAN_RAD_CMD_AUD_ByteStruct  Treble;
};

//----------------------------------------------------------------------------------------//

#endif
