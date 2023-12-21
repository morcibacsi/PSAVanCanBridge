#pragma once

#ifndef _VAN_554_h
    #define _VAN_554_h

#include <stdint.h>

// VANID: 554
const uint16_t VAN_ID_RADIO_TUNER = 0x554;
const uint8_t VAN_ID_RADIO_TUNER_LENGTH = 22;

const uint8_t VAN_RADIO_TUNER_BAND_NONE  = 0;
const uint8_t VAN_RADIO_TUNER_BAND_FM1   = 1;
const uint8_t VAN_RADIO_TUNER_BAND_FM2   = 2;
const uint8_t VAN_RADIO_TUNER_BAND_FM3   = 3;
const uint8_t VAN_RADIO_TUNER_BAND_FMAST = 4;
const uint8_t VAN_RADIO_TUNER_BAND_AM    = 5;
const uint8_t VAN_RADIO_TUNER_BAND_LW    = 6;
const uint8_t VAN_RADIO_TUNER_BAND_PTY   = 7;

const uint8_t VAN_RADIO_TUNER_TUNING_DIRECTION_DOWN = 0;
const uint8_t VAN_RADIO_TUNER_TUNING_DIRECTION_UP = 1;

const uint8_t VAN_RADIO_SOURCE_FREQ_INFO   = 0xD1;
const uint8_t VAN_RADIO_SOURCE_TAPE_DATA   = 0xD2;
const uint8_t VAN_RADIO_SOURCE_PRESET_DATA = 0xD3;
const uint8_t VAN_RADIO_SOURCE_CD_INFO     = 0xD6;

#pragma region Tuner

union VanRadioTunerByte3Struct {
    struct {
        uint8_t band              : 3; // bit 0-2
        uint8_t memory_postition  : 4; // bit 3-6
        uint8_t is_stored         : 1; // bit 7
    } data;
    uint8_t asByte;
};

union VanRadioTunerByte4Struct {
    struct {
        uint8_t tuner_scan_criteria       : 1; // bit 0
        uint8_t tuner_sensitivity         : 1; // bit 1
        uint8_t pty_mode_active           : 1; // bit 2
        uint8_t frequency_scan_status     : 4; // bit 3-6
        uint8_t frequency_scan_direction  : 1; // bit 7
    } data;
    uint8_t asByte;
};

union VanRadioTunerByte6Struct {
    struct {
        uint8_t freq_msb      : 3; // bit 0-2
        uint8_t               : 2; // bit 3-4
        uint8_t country_code  : 3; // bit 5-7
    } data;
    uint8_t asByte;
};

union VanRadioTunerByte7Struct {
    struct {
        uint8_t signal_quality : 4; // bit 0
        uint8_t signal_level   : 4; // bit 1
    } data;
    uint8_t asByte;
};

union VanRadioTunerByte8Struct {
    struct {
        uint8_t rds_selected                  : 1; // bit 0
        uint8_t ta_selected                   : 1; // bit 1
        uint8_t reg_selected                  : 1; // bit 2
        uint8_t ta_message_received           : 1; // bit 3
        uint8_t station_belongs_to_eon        : 1; // bit 4
        uint8_t rds_available                 : 1; // bit 5
        uint8_t ta_available                  : 1; // bit 6
        uint8_t listening_to_traffic_message  : 1; // bit 7
    } data;
    uint8_t asByte;
};

union VanRadioTunerByte11Struct {
    struct {
        uint8_t pty_type              : 4; // bit 0-4
        uint8_t pty_inactive          : 1; // bit 5
        uint8_t pty_selection_active  : 1; // bit 6 - PTY menu is displayed
        uint8_t memory_type           : 1; // bit 7
    } data;
    uint8_t asByte;
};

union VanRadioTunerByte12Struct {
    struct {
        uint8_t pty_selection : 5; // bit 0-4
        uint8_t program_type  : 3; // bit 5-7
    } data;
    uint8_t asByte;
};

union {
    unsigned char frequencySplitToBytes[2];
    struct {
        unsigned int frequency : 16;
    } Frequency;
} VanRadioTunerFrequencyStruct;


// Read left to right in documentation
struct VanRadioTunerStruct {
    uint8_t Header;
    uint8_t Source;
    VanRadioTunerByte3Struct BandAndPosition;
    VanRadioTunerByte4Struct SearchOptions;
    uint8_t Frequency2LSB;
    VanRadioTunerByte6Struct Frequency1MSB;
    VanRadioTunerByte7Struct SignalQuality;
    VanRadioTunerByte8Struct RdsAndTa;
    uint8_t ProgramId1;
    uint8_t ProgramId2;
    VanRadioTunerByte11Struct PTYInfo;
    VanRadioTunerByte12Struct PTYAir;
    uint8_t RDSTextLetter1;
    uint8_t RDSTextLetter2;
    uint8_t RDSTextLetter3;
    uint8_t RDSTextLetter4;
    uint8_t RDSTextLetter5;
    uint8_t RDSTextLetter6;
    uint8_t RDSTextLetter7;
    uint8_t RDSTextLetter8;
    uint8_t CodePageOfRDS;
    uint8_t Footer;
};

union VanRadioTunerPacket {
    VanRadioTunerStruct data;
    uint8_t VanRadioTunerPacket[sizeof(VanRadioTunerStruct)];
};

// Returns the radio frequency displayed on the screen (example: 107.8)
float static GetVanRadioFrequency(uint8_t byte4, uint8_t byte5)
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

const uint8_t VAN_RADIO_TAPE_DIRECTION_SIDE1_A = 0;
const uint8_t VAN_RADIO_TAPE_DIRECTION_SIDE2_B = 1;

const uint8_t VAN_RADIO_TAPE_PLAYBACK_STATUS_NO_CASETTE   = 0;
const uint8_t VAN_RADIO_TAPE_PLAYBACK_STATUS_STOP         = 1;
const uint8_t VAN_RADIO_TAPE_PLAYBACK_STATUS_PAUSE        = 2;
const uint8_t VAN_RADIO_TAPE_PLAYBACK_STATUS_READ         = 3;
const uint8_t VAN_RADIO_TAPE_PLAYBACK_STATUS_FAST_FORWARD = 4;
const uint8_t VAN_RADIO_TAPE_PLAYBACK_STATUS_SEARCH_BLANK = 5;
const uint8_t VAN_RADIO_TAPE_PLAYBACK_STATUS_JUMP_BLANK   = 6;
const uint8_t VAN_RADIO_TAPE_PLAYBACK_STATUS_INTRO_SCAN   = 7;

const uint8_t VAN_RADIO_TAPE_SCROLL_DIRECTION_FORWARD  = 0;
const uint8_t VAN_RADIO_TAPE_SCROLL_DIRECTION_BACKWARD = 1;

const uint8_t VAN_RADIO_TAPE_TYPE_FERRO      = 0;
const uint8_t VAN_RADIO_TAPE_TYPE_CRO2_METAL = 1;
const uint8_t VAN_RADIO_TAPE_TYPE_NOT_USED_1 = 2;
const uint8_t VAN_RADIO_TAPE_TYPE_NOT_USED_2 = 3;

const uint8_t VAN_RADIO_TAPE_DOLBY_TYPE_NONE    = 0;
const uint8_t VAN_RADIO_TAPE_DOLBY_TYPE_DOLBY_B = 1;
const uint8_t VAN_RADIO_TAPE_DOLBY_TYPE_DOLBY_C = 2;
const uint8_t VAN_RADIO_TAPE_DOLBY_TYPE_DOLBY_NR = 3;

union VanRadioTapeByte3Struct {
    struct {
        uint8_t casette_side                : 1; // bit 0     VAN_RADIO_TAPE_DIRECTION
        uint8_t skip_blank                  : 1; // bit 1
        uint8_t playback_status             : 3; // bit 2-4   VAN_RADIO_TAPE_PLAYBACK_STATUS
        uint8_t scroll_direction            : 1; // bit 5     VAN_RADIO_TAPE_SCROLL_DIRECTION
        uint8_t                             : 1; // bit 6
        uint8_t listen_tuner_during_forward : 1; // bit 7
    } data;
    uint8_t asByte;
};

union VanRadioTapeByte4Struct {
    struct {
        uint8_t dolby_type : 2; // bit 0-1  VAN_RADIO_TAPE_DOLBY_TYPE
        uint8_t tape_type  : 2; // bit 2-3  VAN_RADIO_TAPE_TYPE
        uint8_t            : 4; // bit 4-7
    } data;
    uint8_t asByte;
};

struct VanRadioTapeStruct {
    uint8_t Header;
    uint8_t Source;
    VanRadioTapeByte3Struct Status;
    VanRadioTapeByte4Struct TapeType;
    uint8_t Footer;
};

union VanRadioTapePacket {
    VanRadioTapeStruct data;
    uint8_t VanRadioTapePacket[sizeof(VanRadioTapeStruct)];
};

#pragma endregion

#pragma region Preset info

union VanRadioPresetByte3Struct {
    struct {
        uint8_t preset_number      : 4; // bit 0-3
        uint8_t band_type          : 3; // bit 4
        uint8_t is_preset          : 1; // bit 5-7
    } data;
    uint8_t asByte;
};

struct VanRadioPresetStruct {
    uint8_t Header;
    uint8_t Source;
    VanRadioPresetByte3Struct Status;
    uint8_t RDSTextLetter1;
    uint8_t RDSTextLetter2;
    uint8_t RDSTextLetter3;
    uint8_t RDSTextLetter4;
    uint8_t RDSTextLetter5;
    uint8_t RDSTextLetter6;
    uint8_t RDSTextLetter7;
    uint8_t RDSTextLetter8;
    uint8_t Footer;
};

union VanRadioPresetPacket {
    VanRadioPresetStruct data;
    uint8_t VanRadioPresetPacket[sizeof(VanRadioPresetStruct)];
};

#pragma endregion

#pragma region CD

const uint8_t VAN_RADIO_CD_REPETITION_MODE_NORMAL = 0;
const uint8_t VAN_RADIO_CD_REPETITION_MODE_TRACK  = 1;
const uint8_t VAN_RADIO_CD_REPETITION_MODE_DISC   = 2;

const uint8_t VAN_RADIO_CD_PLAY_MODE_NORMAL      = 0;
const uint8_t VAN_RADIO_CD_PLAY_MODE_RANDOM_PLAY = 1;
const uint8_t VAN_RADIO_CD_PLAY_MODE_RESERVED_RANDOM_DISK = 2;
const uint8_t VAN_RADIO_CD_PLAY_MODE_RESERVED_RANDOM_DISK_AND_TRACKS = 3;

const uint8_t VAN_RADIO_CD_PLAYBACK_STATUS_NO_DISK = 0;
const uint8_t VAN_RADIO_CD_PLAYBACK_STATUS_STOP    = 1;
const uint8_t VAN_RADIO_CD_PLAYBACK_STATUS_PAUSE   = 2;
const uint8_t VAN_RADIO_CD_PLAYBACK_STATUS_READING = 3;
const uint8_t VAN_RADIO_CD_PLAYBACK_STATUS_FAST_FORWARD = 4;
const uint8_t VAN_RADIO_CD_PLAYBACK_STATUS_FAST_REVERSE = 5;
const uint8_t VAN_RADIO_CD_PLAYBACK_STATUS_INTRO        = 7;

union VanRadioCDByte3Struct {
    struct {
        uint8_t play_mode   : 2; // bit 0-1     VAN_RADIO_CD_PLAY_MODE
        uint8_t repeat_mode : 2; // bit 2-3     VAN_RADIO_CD_REPETITION_MODE
        uint8_t             : 4; // bit 4-7
    } data;
    uint8_t asByte;
};

union VanRadioCDByte4Struct {
    struct {
        uint8_t playback_status : 3; // bit 0-2     VAN_RADIO_CD_PLAYBACK_STATUS
        uint8_t test_cd         : 1; // bit 3
        uint8_t access_cd       : 1; // bit 4
        uint8_t                 : 3; // bit 5-7
    } data;
    uint8_t asByte;
};

union VanRadioCDByte5Struct {
    struct {
        uint8_t                      : 6; // bit 0-5
        uint8_t cd_error             : 1; // bit 6
        uint8_t cd_temperature_error : 1; // bit 7
    } data;
    uint8_t asByte;
};

struct VanRadioCDStruct {
    uint8_t Header;
    uint8_t Source;
    VanRadioCDByte3Struct PlayMode;
    VanRadioCDByte4Struct CDStatus1;
    VanRadioCDByte5Struct CDStatus2;
    uint8_t CurrentMinutes;
    uint8_t CurrentSeconds;
    uint8_t CurrentTrack;
    uint8_t TrackCount;
    uint8_t TotalDiskMinutes;
    uint8_t TotalDiskSeconds;
    uint8_t TotalTrackMinutes;
    uint8_t TotalTrackSeconds;
    uint8_t RemainingDiskMinutes;
    uint8_t RemainingDiskSeconds;
    uint8_t RemainingTrackMinutes;
    uint8_t RemainingTrackSeconds;
    uint8_t DelayToNextTrack;
    uint8_t Footer;
};

union VanRadioCDPacket {
    VanRadioCDStruct data;
    uint8_t VanRadioCDPacket[sizeof(VanRadioCDStruct)];
};

#pragma endregion

#endif
