// CanDash4Structs.h
#pragma once

#ifndef _CanDash4Structs_h
    #define _CanDash4Structs_h

#include "AbstractCanMessageSender.h"
#include "PacketGenerator.h"

// CANID: 161
const uint16_t CAN_ID_DASH4 = 0x161;

// Read left to right in documentation
typedef struct CanDash4Struct {
    uint8_t Field1;
    uint8_t Field2;
    uint8_t Field3;
    uint8_t FuelLevel;
    uint8_t Field5;
    uint8_t Field6;
    uint8_t Field7;
    uint8_t Field8;
};

typedef union CanDash4Packet {
    CanDash4Struct data;
    uint8_t CanDash4Packet[sizeof(CanDash4Struct)];
};


class CanDash4PacketSender
{
    AbstractCanMessageSender * canMessageSender;

    public:
    CanDash4PacketSender(AbstractCanMessageSender * object)
    {
        canMessageSender = object;
    }

    void SendData(uint8_t fuelLevel)
    {
        PacketGenerator<CanDash4Packet> generator;
        generator.packet.data.FuelLevel = fuelLevel;

        unsigned char *serializedPacket = generator.GetSerializedPacket();
        canMessageSender->SendMessage(CAN_ID_DASH4, 0, sizeof(CanDash4Packet), serializedPacket);
    }
};
#endif
