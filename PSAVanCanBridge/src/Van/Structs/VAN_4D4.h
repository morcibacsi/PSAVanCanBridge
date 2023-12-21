#pragma once

#ifndef _VAN_4D4_h
    #define _VAN_4D4_h

#include <stdint.h>

// VANID: 4D4
const uint16_t VAN_ID_RADIO_INFO = 0x4D4;

const uint8_t VAN_ID_RADIO_INFO_LENGTH = 11;
const uint16_t VAN_ID_RADIO_INFO_INTERVAL = 100;

const uint8_t VAN_RADIO_SRC_NONE            = 0;
const uint8_t VAN_RADIO_SRC_TUNER           = 1;
const uint8_t VAN_RADIO_SRC_K7_MD_CD        = 2;
const uint8_t VAN_RADIO_SRC_CDC             = 3;
const uint8_t VAN_RADIO_SRC_PHONE           = 4;
const uint8_t VAN_RADIO_SRC_VOCAL_SYNTHESIS = 5;
const uint8_t VAN_RADIO_SRC_RESERVED        = 6;
const uint8_t VAN_RADIO_SRC_TV              = 7;
const uint8_t VAN_RADIO_SRC_AUX             = 8;

//Read right to left in documentation

union VanRadioInfoByte2Struct {
    struct {
        uint8_t muted               : 1; // bit 0
        uint8_t muted_request       : 1; // bit 1
        uint8_t auto_volume_active  : 1; // bit 2
        uint8_t speaker_count       : 1; // bit 3
        uint8_t loudness_on         : 1; // bit 4
        uint8_t properties_open     : 1; // bit 5
        uint8_t                     : 2; // bit 6-7
    } data;
    uint8_t asByte;
};

union VanRadioInfoByte3Struct {
    struct {
        uint8_t power_on            : 1; // bit 0
        uint8_t activation_request  : 1; // bit 1
        uint8_t stop_request        : 1; // bit 2
        uint8_t tape_request        : 1; // bit 3 indicates tape insertion on screen
        uint8_t cd_request          : 1; // bit 4 indicates cd insertion on screen
        uint8_t phone_request       : 1; // bit 5
        uint8_t                     : 1; // bit 6
        uint8_t vin_ok              : 1; // bit 7
    } data;
    uint8_t asByte;
};

union VanRadioInfoByte5Struct {
    struct {
        uint8_t source              : 4; // bit 0-3 VAN_RADIO_SRC
        uint8_t tuner_available     : 1; // bit 4
        uint8_t tape_available      : 1; // bit 5 (TODO seems like it is switched around? need to check with tape unit)
        uint8_t cd_available        : 1; // bit 6
        uint8_t                     : 1; // bit 7
    } data;
    uint8_t asByte;
};

union VanRadioInfoAudioSettingsByteStruct {
    struct {
        uint8_t value    : 7; // bit 0-6
        uint8_t updating : 1; // bit 7
    } data;
    uint8_t asByte;
};

// Read left to right in documentation
struct VanRadioInfoStruct {
    uint8_t Header;
    VanRadioInfoByte2Struct Status;
    VanRadioInfoByte3Struct General;
    uint8_t Consumption;
    VanRadioInfoByte5Struct Source;
    VanRadioInfoAudioSettingsByteStruct Volume;
    VanRadioInfoAudioSettingsByteStruct Balance;
    VanRadioInfoAudioSettingsByteStruct Fader;
    VanRadioInfoAudioSettingsByteStruct Bass;
    VanRadioInfoAudioSettingsByteStruct Treble;
    uint8_t Footer;
};

union VanRadioInfoPacket {
    VanRadioInfoStruct data;
    uint8_t VanRadioInfoPacket[sizeof(VanRadioInfoStruct)];
};

#endif
