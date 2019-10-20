// CanRadioVolumeStructs.h
#pragma once

#ifndef _CanRadioVolumeStructs_h
    #define _CanRadioVolumeStructs_h

#include "AbstractCanMessageSender.h"
#include "PacketGenerator.h"

// CANID: 1A5
const uint16_t CAN_ID_RADIO_VOLUME = 0x1A5;

const uint8_t CAN_RADIO_VOLUME_SCROLLING_ON  = B000;
const uint8_t CAN_RADIO_VOLUME_SCROLLING_OFF = B111;

typedef struct {
    uint8_t volume       : 5; // bit 0-4
    uint8_t scrolling    : 3; // bit 5-7
} CanRadioVolumeByte1Struct;

// Read left to right in documentation
typedef struct CanRadioAudioSettingsStruct {
    CanRadioVolumeByte1Struct Volume;
};

typedef union CanRadioAudioSettingsPacket {
    CanRadioAudioSettingsStruct data;
    uint8_t CanRadioAudioSettingsPacket[sizeof(CanRadioAudioSettingsStruct)];
};

#pragma region Sender class
class CanRadioVolumePacketSender
{
    AbstractCanMessageSender * canMessageSender;

public:
    CanRadioVolumePacketSender(AbstractCanMessageSender * object)
    {
        canMessageSender = object;
    }

    void Send(uint8_t volume, bool changing)
    {
        PacketGenerator<CanRadioAudioSettingsPacket> generator;

        generator.packet.data.Volume.volume = volume;
        generator.packet.data.Volume.scrolling = changing ? CAN_RADIO_VOLUME_SCROLLING_ON : CAN_RADIO_VOLUME_SCROLLING_OFF;

        unsigned char *serializedPacket = generator.GetSerializedPacket();
        canMessageSender->SendMessage(CAN_ID_RADIO_VOLUME, 0, sizeof(CanRadioAudioSettingsPacket), serializedPacket);
    }
};
#pragma endregion

#endif
