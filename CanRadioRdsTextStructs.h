// CanRadioRdsTextStructs.h
#pragma once

#ifndef _CanRadioRdsTextStructs_h
    #define _CanRadioRdsTextStructs_h

#include "AbstractCanMessageSender.h"
#include "PacketGenerator.h"

// CANID: 2A5
const uint16_t CAN_ID_RADIO_RDS_TEXT = 0x2A5;

// Read left to right in documentation
typedef struct CanRadioRdsTextStruct {
    uint8_t Letter1;
    uint8_t Letter2;
    uint8_t Letter3;
    uint8_t Letter4;
    uint8_t Letter5;
    uint8_t Letter6;
    uint8_t Letter7;
    uint8_t Letter8;
};

typedef union CanRadioRdsTextPacket {
    CanRadioRdsTextStruct data;
    uint8_t CanRadioRdsTextPacket[sizeof(CanRadioRdsTextStruct)];
};

#pragma region Sender class
class CanRadioRdsTextPacketSender
{
    AbstractCanMessageSender * canMessageSender;

public:
    CanRadioRdsTextPacketSender(AbstractCanMessageSender * object)
    {
        canMessageSender = object;
    }

    void Send(uint8_t rdsText[8])
    {
        PacketGenerator<CanRadioRdsTextPacket> generator;

        generator.packet.data.Letter1 = rdsText[0];
        generator.packet.data.Letter2 = rdsText[1];
        generator.packet.data.Letter3 = rdsText[2];
        generator.packet.data.Letter4 = rdsText[3];
        generator.packet.data.Letter5 = rdsText[4];
        generator.packet.data.Letter6 = rdsText[5];
        generator.packet.data.Letter7 = rdsText[6];
        generator.packet.data.Letter8 = rdsText[7];

        unsigned char *serializedPacket = generator.GetSerializedPacket();
        canMessageSender->SendMessage(CAN_ID_RADIO_RDS_TEXT, 0, sizeof(CanRadioRdsTextPacket), serializedPacket);
    }
};
#pragma endregion

#endif
