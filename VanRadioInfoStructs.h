// VanRadioInfoStructs.h
#pragma once

#ifndef _VanRadioInfoStructs_h
#define _VanRadioInfoStructs_h

// VANID: 4D4
const uint16_t VAN_ID_RADIO_INFO = 0x4D4;

//Read right to left in documentation

typedef struct {
    uint8_t                            : 1; // bit 0
    uint8_t                            : 1; // bit 1
    uint8_t auto_volume                : 1; // bit 2
    uint8_t                            : 1; // bit 3
    uint8_t loudness_on                : 1; // bit 4
    uint8_t audio_properties_menu_open : 1; // bit 5
    uint8_t                            : 1; // bit 6
    uint8_t                            : 1; // bit 7
} VanRadioInfoByte1Struct;

typedef struct {
    uint8_t power_on            : 1; // bit 0
    uint8_t                     : 1; // bit 1
    uint8_t                     : 1; // bit 2
    uint8_t                     : 1; // bit 3
    uint8_t                     : 1; // bit 4
    uint8_t                     : 1; // bit 5
    uint8_t                     : 1; // bit 6
    uint8_t                     : 1; // bit 7
} VanRadioInfoByte2Struct;

typedef struct {
    uint8_t radio               : 1; // bit 0
    uint8_t tape                : 1; // bit 1
    uint8_t cd                  : 1; // bit 2
    uint8_t                     : 1; // bit 3
    uint8_t                     : 1; // bit 4
    uint8_t                     : 1; // bit 5
    uint8_t                     : 1; // bit 6
    uint8_t                     : 1; // bit 7
} VanRadioInfoByte4Struct;

typedef struct {
    uint8_t value    : 7; // bit 0-6
    uint8_t updating : 1; // bit 7
} VanRadioInfoAudioSettingsByteStruct;

// Read left to right in documentation
typedef struct VanRadioInfoStruct {
    uint8_t Header;
    VanRadioInfoByte1Struct AudioProperties;
    VanRadioInfoByte2Struct Power;
    uint8_t Field3;
    uint8_t Source;
    VanRadioInfoAudioSettingsByteStruct Volume;
    VanRadioInfoAudioSettingsByteStruct Balance;
    VanRadioInfoAudioSettingsByteStruct Fader;
    VanRadioInfoAudioSettingsByteStruct Bass;
    VanRadioInfoAudioSettingsByteStruct Treble;
    uint8_t Footer;
};

typedef union VanRadioInfoPacket {
    VanRadioInfoStruct data;
    uint8_t VanRadioInfoPacket[sizeof(VanRadioInfoStruct)];
};

#endif
