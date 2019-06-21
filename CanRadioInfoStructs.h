// CanRadioInfoStructs.h
#pragma once

#ifndef _CanRadioInfoStructs_h
    #define _CanRadioInfoStructs_h

// CANID: 265
const unsigned long CAN_ID_RADIO_INFO = 0x265;

typedef struct {
    uint8_t reg_enabled  : 1; // bit 0
    uint8_t traffic_info : 1; // bit 1
    uint8_t ta_found     : 1; // bit 2
    uint8_t unknown3     : 1; // bit 3
    uint8_t ta_active    : 1; // bit 4
    uint8_t rds_found    : 1; // bit 5
    uint8_t unknown6     : 1; // bit 6
    uint8_t rds_active   : 1; // bit 7
} CanRadioInfoByte1Struct;

typedef struct {
    uint8_t                : 4; // bit 0-3
    uint8_t pty_info       : 2; // bit 4-5
    uint8_t pty_menu_open  : 1; // bit 6
    uint8_t pty            : 1; // bit 7
} CanRadioInfoByte2Struct;

// Read left to right in documentation
typedef struct CanRadioInfoStruct {
    CanRadioInfoByte1Struct Features;
    CanRadioInfoByte2Struct Pty;
    uint8_t PtyMenuItem;
    uint8_t CurrentStationPty;
};

typedef union CanRadioInfoPacket {
    CanRadioInfoStruct data;
    uint8_t CanRadioInfoPacket[sizeof(CanRadioInfoStruct)];
};

#pragma region Sender class
class CanRadioInfoPacketSender
{
    AbstractCanMessageSender * canMessageSender;

public:
    CanRadioInfoPacketSender(AbstractCanMessageSender * object)
    {
        canMessageSender = object;
    }

    void Send(uint8_t rds, uint8_t ta)
    {
        PacketGenerator<CanRadioInfoPacket> generator;

        generator.packet.data.Features.rds_active = rds;
        generator.packet.data.Features.rds_found = rds;
        generator.packet.data.Features.ta_active = ta;
        generator.packet.data.Features.ta_found = ta;

        unsigned char *serializedPacket = generator.GetSerializedPacket();
        canMessageSender->SendMessage(CAN_ID_RADIO_INFO, 0, sizeof(CanRadioInfoPacket), serializedPacket);
    }
};
#pragma endregion

#endif
