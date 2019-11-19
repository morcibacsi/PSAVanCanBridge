// CanRadioAudioSettingsStructs.h
#pragma once

#ifndef _CanRadioAudioSettingsStructs_h
    #define _CanRadioAudioSettingsStructs_h

// CANID: 1E5
const uint16_t CAN_ID_RADIO_AUDIO_SETTINGS = 0x1E5;

const uint8_t CAN_RADIO_AMBIENCE_NONE       = B00011;
const uint8_t CAN_RADIO_AMBIENCE_CLASSICAL  = B00111;
const uint8_t CAN_RADIO_AMBIENCE_JAZZ_BLUES = B01011;
const uint8_t CAN_RADIO_AMBIENCE_POP        = B01111;
const uint8_t CAN_RADIO_AMBIENCE_VOCAL      = B10011;
const uint8_t CAN_RADIO_AMBIENCE_TECHNO     = B10111;

typedef struct {
    uint8_t value   : 7; // bit 0-6
    uint8_t show    : 1; // bit 7
} CanRadioInfoAudioSettingsStruct;

typedef struct {
    uint8_t value_of_automatic_volume    : 3; // bit 0-2
    uint8_t unknown1                     : 1; // bit 3
    uint8_t show_automatic_volume        : 1; // bit 4
    uint8_t unknown5                     : 1; // bit 5
    uint8_t value_of_loudness_correction : 1; // bit 6
    uint8_t show_loudness_correction     : 1; // bit 7
} CanRadioInfoAudioSettingsByte6Struct;

typedef struct {
    uint8_t ambience      : 5; // bit 0-4
    uint8_t unknown5      : 1; // bit 5
    uint8_t show_ambience : 1; // bit 6
    uint8_t unknown7      : 1; // bit 7
} CanRadioInfoAudioSettingsByte7Struct;


// Read left to right in documentation
typedef struct CanRadioAudioSettingsStruct {
    CanRadioInfoAudioSettingsStruct BalanceLeftRight;
    CanRadioInfoAudioSettingsStruct BalanceRearFront;
    CanRadioInfoAudioSettingsStruct Bass;
    uint8_t Field4;
    CanRadioInfoAudioSettingsStruct Treble;
    CanRadioInfoAudioSettingsByte6Struct Features;
    CanRadioInfoAudioSettingsByte7Struct Ambience;
};

typedef union CanRadioAudioSettingsPacket {
    CanRadioAudioSettingsStruct data;
    uint8_t CanRadioAudioSettingsPacket[sizeof(CanRadioAudioSettingsStruct)];
};

uint8_t GetCanAudioSettingValueFromCanMessage(int numberInCanMessage)
{
    return numberInCanMessage - 0x3F;
}

int GetCanAudioSettingValueToDisplay(uint8_t numberFromMinus9To9)
{
    if (numberFromMinus9To9 < -9)
    {
        return 0x36;
    }
    if (numberFromMinus9To9 > 9)
    {
        return 0x48;
    }
    return numberFromMinus9To9 + 0x3F;
}

#endif
