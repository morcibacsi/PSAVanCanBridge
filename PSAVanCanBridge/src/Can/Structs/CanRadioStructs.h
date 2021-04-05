// CanRadioStructs.h
#pragma once

#ifndef _CanRadioStructs_h
    #define _CanRadioStructs_h

#include "../AbstractCanMessageSender.h"
#include "../../Helpers/PacketGenerator.h"

// CANID: 165
const uint16_t CAN_ID_RADIO = 0x165;

const uint8_t CAN_RADIO_SOURCE_BLUETOOTH  = B111;
const uint8_t CAN_RADIO_SOURCE_USB        = B110;
const uint8_t CAN_RADIO_SOURCE_AUX2       = B101;
const uint8_t CAN_RADIO_SOURCE_AUX1       = B100;
const uint8_t CAN_RADIO_SOURCE_CD_CHANGER = B011;
const uint8_t CAN_RADIO_SOURCE_CD         = B010;
const uint8_t CAN_RADIO_SOURCE_TUNER      = B001;

typedef struct {
    uint8_t        : 1; // bit 0
    uint8_t        : 1; // bit 1
    uint8_t        : 1; // bit 2
    uint8_t        : 1; // bit 3
    uint8_t        : 1; // bit 4
    uint8_t mute   : 1; // bit 5
    uint8_t        : 1; // bit 6
    uint8_t radio_enabled : 1; // bit 7
} CanRadioByte1Struct;

typedef struct {
    uint8_t        : 1; // bit 0
    uint8_t        : 1; // bit 1
    uint8_t        : 1; // bit 2
    uint8_t        : 1; // bit 3
    uint8_t source : 3; // bit 4
    uint8_t        : 1; // bit 7
} CanRadioByte3Struct;

// Read left to right in documentation
struct CanRadioStruct {
    CanRadioByte1Struct Field1;
    uint8_t CdInfo;
    CanRadioByte3Struct Source;
    uint8_t Field4;
};

union CanRadioPacket {
    CanRadioStruct data;
    uint8_t CanRadioPacket[sizeof(CanRadioStruct)];
};

#pragma region Sender class
class CanRadioPacketSender
{
    AbstractCanMessageSender * canMessageSender;

public:
    CanRadioPacketSender(AbstractCanMessageSender * object)
    {
        canMessageSender = object;
    }

    void Send(bool enabled, uint8_t source)
    {
        PacketGenerator<CanRadioPacket> generator;

        generator.packet.data.Field1.radio_enabled = enabled ? 1 : 0;
        generator.packet.data.Source.source = source;

        unsigned char *serializedPacket = generator.GetSerializedPacket();
        canMessageSender->SendMessage(CAN_ID_RADIO, 0, sizeof(CanRadioPacket), serializedPacket);
    }
};
#pragma endregion

#endif
