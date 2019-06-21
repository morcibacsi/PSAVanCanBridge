// CanRadioTunerStructs.h
#pragma once

#ifndef _CanRadioTunerStructs_h
    #define _CanRadioTunerStructs_h

// CANID: 225
const unsigned long CAN_ID_RADIO_TUNER = 0x225;

const uint8_t CAN_RADIO_TUNER_BAND_NONE  = 0;
const uint8_t CAN_RADIO_TUNER_BAND_FM1   = 1;
const uint8_t CAN_RADIO_TUNER_BAND_FM2   = 2;
const uint8_t CAN_RADIO_TUNER_BAND_FMAST = 4;
const uint8_t CAN_RADIO_TUNER_BAND_AM    = 5;

const uint8_t CAN_RADIO_TUNER_TUNING_DIRECTION_DOWN = 0;
const uint8_t CAN_RADIO_TUNER_TUNING_DIRECTION_UP = 1;

typedef struct {
    uint8_t pty_active                    : 1; // bit 0
    uint8_t frequency_tuning_active       : 1; // bit 1
    uint8_t rds_search_active             : 1; // bit 2
    uint8_t autoscan_active               : 1; // bit 3
    uint8_t unknown4                      : 1; // bit 4
    uint8_t direction_of_frequency_tuning : 1; // bit 5 / 0 - down, 1 - up
    uint8_t unknown6                      : 1; // bit 6
    uint8_t station_list_open             : 1; // bit 7
} CanRadioTunerByte1Struct;

typedef struct {
    uint8_t                   : 1; // bit 0
    uint8_t                   : 1; // bit 1
    uint8_t                   : 1; // bit 2
    uint8_t                   : 1; // bit 3
    uint8_t position_of_band  : 3; // bit 4-6
    uint8_t                   : 1; // bit 7
} CanRadioTunerByte2Struct;

typedef struct {
    uint8_t       : 1; // bit 0
    uint8_t       : 1; // bit 1
    uint8_t       : 1; // bit 2
    uint8_t       : 1; // bit 3
    uint8_t band  : 3; // bit 4-6
    uint8_t       : 1; // bit 7
} CanRadioTunerByte3Struct;

typedef struct {
    unsigned short int frequency : 12;
    uint8_t : 1;
    uint8_t : 1;
    uint8_t : 1;
    uint8_t : 1;
} CanRadioTunerFrequencyByte45Struct;


union {
    unsigned char frequencySplitToBytes[2];
    struct {
        unsigned int frequency : 12;
    } Frequency;
} CanRadioTunerFrequencyStruct;


// Read left to right in documentation
typedef struct CanRadioTunerStruct {
    CanRadioTunerByte1Struct FeatureActivityState;
    CanRadioTunerByte2Struct BandPosition;
    CanRadioTunerByte3Struct Band;
    uint8_t Frequency1;
    uint8_t Frequency2;
};

typedef union CanRadioTunerPacket {
    CanRadioTunerStruct data;
    uint8_t CanRadioTunerPacket[sizeof(CanRadioTunerStruct)];
};

unsigned int GetCanRadioFrequencyToDisplay(float frequency)
{
    return round((frequency - 50) / 0.05);
}

#pragma region Sender class
class CanRadioTunerMessageSender
{
    AbstractCanMessageSender * canMessageSender;

public:
    CanRadioTunerMessageSender(AbstractCanMessageSender * object)
    {
        canMessageSender = object;
    }

    void Send(uint8_t band, float radioFrequency, uint8_t bandPosition)
    {
        PacketGenerator<CanRadioTunerPacket> generator;

        generator.packet.data.Band.band = band;

        CanRadioTunerFrequencyStruct.Frequency.frequency = GetCanRadioFrequencyToDisplay(radioFrequency);
        generator.packet.data.Frequency1 = CanRadioTunerFrequencyStruct.frequencySplitToBytes[1];
        generator.packet.data.Frequency2 = CanRadioTunerFrequencyStruct.frequencySplitToBytes[0];
        generator.packet.data.BandPosition.position_of_band = bandPosition;

        unsigned char *serializedPacket = generator.GetSerializedPacket();
        canMessageSender->SendMessage(CAN_ID_RADIO_TUNER, 0, sizeof(CanRadioTunerPacket), serializedPacket);
    }
};
#pragma endregion

#endif
