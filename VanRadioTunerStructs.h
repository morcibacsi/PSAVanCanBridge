// VanRadioTunerStructs.h
#pragma once

#ifndef _VanRadioTunerStructs_h
    #define _VanRadioTunerMessageStructs_h

// VANID: 554
const unsigned long VAN_ID_RADIO_TUNER = 0x554;

const uint8_t VAN_RADIO_TUNER_BAND_NONE  = 0;
const uint8_t VAN_RADIO_TUNER_BAND_FM1   = 1;
const uint8_t VAN_RADIO_TUNER_BAND_FM2   = 2;
const uint8_t VAN_RADIO_TUNER_BAND_FMAST = 4;
const uint8_t VAN_RADIO_TUNER_BAND_AM    = 5;

const uint8_t VAN_RADIO_TUNER_TUNING_DIRECTION_DOWN = 0;
const uint8_t VAN_RADIO_TUNER_TUNING_DIRECTION_UP = 1;

const uint8_t VAN_RADIO_SOURCE_FREQ_INFO   = 0xD1;
const uint8_t VAN_RADIO_SOURCE_TAPE_DATA   = 0xD2;
const uint8_t VAN_RADIO_SOURCE_PRESET_DATA = 0xD3;
const uint8_t VAN_RADIO_SOURCE_CD_INFO     = 0xD6;


#pragma region Tuner

typedef struct {
    uint8_t band              : 3; // bit 0-2
    uint8_t position_of_band  : 4; // bit 3-6
    uint8_t                   : 1; // bit 7
} VanRadioTunerByte2Struct;

typedef struct {
    uint8_t                           : 1; // bit 0
    uint8_t                           : 1; // bit 1
    uint8_t                           : 1; // bit 2
    uint8_t manual_scan_in_progress   : 1; // bit 3
    uint8_t frequency_scan_is_running : 1; // bit 4
    uint8_t                           : 1; // bit 5
    uint8_t                           : 1; // bit 6
    uint8_t frequency_scan_direction  : 1; // bit 7
} VanRadioTunerByte3Struct;

typedef struct {
    uint8_t rds_active : 1; // bit 0
    uint8_t ta_active  : 1; // bit 1
    uint8_t            : 1; // bit 2
    uint8_t            : 1; // bit 3
    uint8_t            : 1; // bit 4
    uint8_t            : 1; // bit 5
    uint8_t            : 1; // bit 6
    uint8_t            : 1; // bit 7
} VanRadioTunerByte7Struct;

union {
    unsigned char frequencySplitToBytes[2];
    struct {
        unsigned int frequency : 16;
    } Frequency;
} VanRadioTunerFrequencyStruct;


// Read left to right in documentation
typedef struct VanRadioTunerStruct {
    uint8_t Header;
    uint8_t Source;
    VanRadioTunerByte2Struct BandAndPosition;
    VanRadioTunerByte3Struct SearchOptions;
    uint8_t Frequency2;
    uint8_t Frequency1;
    uint8_t Byte6;
    VanRadioTunerByte7Struct RdsAndTa;
    uint8_t Footer;
};

typedef union VanRadioTunerPacket {
    VanRadioTunerStruct data;
    uint8_t VanRadioTunerPacket[sizeof(VanRadioTunerStruct)];
};

// Returns the radio frequency displayed on the screen (example: 107.8)
float GetVanRadioFrequency(uint8_t byte4, uint8_t byte5)
{
    /*
    frequency is stored in the 4th and 5th byte in reverse order
    so in the example frame 3A 03 becomes 03 3A  which equals to 826 and if we apply the formula below it translates to 91.30 Mhz
    example frame:
        55 4B-86 D1 04 88 3A 03 00 05 FF FF A1 00 00 00 00 00 00 00 00 00 00 86
    */
    VanRadioTunerFrequencyStruct.frequencySplitToBytes[0] = byte4;
    VanRadioTunerFrequencyStruct.frequencySplitToBytes[1] = byte5;

    return (VanRadioTunerFrequencyStruct.Frequency.frequency / 20.0) + 50;
}

#pragma endregion

#pragma region Tape

const uint8_t VAN_RADIO_TAPE_DIRECTION_SIDE1 = 0;
const uint8_t VAN_RADIO_TAPE_DIRECTION_SIDE2 = 1;
const uint8_t VAN_RADIO_TAPE_PLAY_ON  = 3;
const uint8_t VAN_RADIO_TAPE_PLAY_OFF = 0;

typedef struct {
    uint8_t casette_side      : 1; // bit 0
    uint8_t unknown1          : 1; // bit 1
    uint8_t play_in_progress  : 2; // bit 2-3
    uint8_t fast_forward      : 1; // bit 4
    uint8_t                   : 3; // bit 5-7
} VanRadioTapeByte2Struct;

typedef struct VanRadioTapeStruct {
    uint8_t Header;
    uint8_t Source;
    VanRadioTapeByte2Struct Status;
    uint8_t Byte3;
    uint8_t Footer;
};

typedef union VanRadioTapePacket {
    VanRadioTapeStruct data;
    uint8_t VanRadioTapePacket[sizeof(VanRadioTapeStruct)];
};

#pragma endregion

#pragma region Preset info

typedef struct {
    uint8_t preset_number      : 4; // bit 0-3
    uint8_t rds_text_available : 1; // bit 4
    uint8_t                    : 2; // bit 5-6
    uint8_t is_preset          : 1; // bit 5-7
} VanRadioPresetByte2Struct;

typedef struct VanRadioPresetStruct {
    uint8_t Header;
    uint8_t Source;
    VanRadioPresetByte2Struct Status;
    uint8_t Byte3;
    uint8_t Footer;
};

typedef union VanRadioPresetPacket {
    VanRadioPresetStruct data;
    uint8_t VanRadioPresetPacket[sizeof(VanRadioPresetStruct)];
};

#pragma endregion

#endif
