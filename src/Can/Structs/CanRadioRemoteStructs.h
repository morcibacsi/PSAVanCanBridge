// CanRadioRemoteStructs.h
#pragma once

#ifndef _CanRadioRemoteStructs_h
    #define _CanRadioRemoteStructs_h

#include "../AbstractCanMessageSender.h"
#include "../../Helpers/PacketGenerator.h"

// CANID: 21F
const uint16_t CAN_ID_RADIO_REMOTE = 0x21F;

// Read right to left in documentation
typedef struct {
    uint8_t unknown0            : 1; // bit 0
    uint8_t SourcePressed       : 1; // bit 1
    uint8_t VolumeDownPressed   : 1; // bit 2
    uint8_t VolumeUpPressed     : 1; // bit 3
    uint8_t unknown4            : 1; // bit 4
    uint8_t unknown5            : 1; // bit 5
    uint8_t SeekDownPressed     : 1; // bit 6
    uint8_t Seek_UpPressed      : 1; // bit 7
} CanRadioRemoteButtonStruct;


// Read left to right in documentation
typedef struct CanRadioRemoteStruct {
    CanRadioRemoteButtonStruct RemoteButton;
    uint8_t ScrollPosition;
    uint8_t Field2;
};

typedef union CanRadioRemotePacket {
    CanRadioRemoteStruct data;
    uint8_t CanRadioRemotePacket[sizeof(CanRadioRemoteStruct)];
};

#pragma region Sender class
class CanRadioRemoteButtonPacketSender
{
    AbstractCanMessageSender * canMessageSender;

public:
    CanRadioRemoteButtonPacketSender(AbstractCanMessageSender * object)
    {
        canMessageSender = object;
    }

    void Send(CanRadioRemoteButtonStruct button, uint8_t scrollPosition)
    {
        PacketGenerator<CanRadioRemotePacket> generator;

        generator.packet.data.RemoteButton = button;
        generator.packet.data.ScrollPosition = scrollPosition;

        unsigned char *serializedPacket = generator.GetSerializedPacket();
        canMessageSender->SendMessage(CAN_ID_RADIO_REMOTE, 0, sizeof(CanRadioRemotePacket), serializedPacket);
    }

    void SendAsByte(uint8_t button, uint8_t scrollPosition)
    {
        PacketGenerator<CanRadioRemotePacket> generator;

        unsigned char *serializedPacket = generator.GetSerializedPacket();

        serializedPacket[0] = button;
        serializedPacket[1] = scrollPosition;

        canMessageSender->SendMessage(CAN_ID_RADIO_REMOTE, 0, sizeof(CanRadioRemotePacket), serializedPacket);
    }
};
#pragma endregion


#endif
