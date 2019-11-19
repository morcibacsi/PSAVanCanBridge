// CanTrip1Structs.h
#pragma once

#ifndef _CanTrip1Structs_h
    #define _CanTrip1Structs_h

#include "../AbstractCanMessageSender.h"
#include "../../Helpers/PacketGenerator.h"

// CANID: 2A1
const uint16_t CAN_ID_TRIP1 = 0x2A1;

// Read left to right in documentation
typedef struct CanTrip1Struct {
    uint8_t AverageSpeed;
    uint8_t MileageAfterResetByte1;
    uint8_t MileageAfterResetByte2;
    uint8_t LitersPer100KmByte1;
    uint8_t LitersPer100KmByte2;
    uint8_t Field6;
    uint8_t Field7;
};

typedef union CanTrip1Packet {
    CanTrip1Struct data;
    uint8_t CanTrip1Packet[sizeof(CanTrip1Struct)];
};

#pragma region Sender class
class CanTrip1PacketSender
{
    AbstractCanMessageSender * canMessageSender;

public:
    CanTrip1PacketSender(AbstractCanMessageSender * object)
    {
        canMessageSender = object;
    }

    void SendTripInfo(int mileageAfterReset, uint8_t lper100km, uint8_t averageSpeed)
    {
        PacketGenerator<CanTrip1Packet> generator;

        IntValueOnTwoBytes.value = mileageAfterReset;
        generator.packet.data.MileageAfterResetByte1 = IntValueOnTwoBytes.bytes[1];
        generator.packet.data.MileageAfterResetByte2 = IntValueOnTwoBytes.bytes[0];

        IntValueOnTwoBytes.value = lper100km;
        generator.packet.data.LitersPer100KmByte1 = IntValueOnTwoBytes.bytes[1];
        generator.packet.data.LitersPer100KmByte2 = IntValueOnTwoBytes.bytes[0];

        generator.packet.data.AverageSpeed = averageSpeed;

        unsigned char *serializedPacket = generator.GetSerializedPacket();
        canMessageSender->SendMessage(CAN_ID_TRIP1, 0, sizeof(CanTrip1Packet), serializedPacket);
    }

};
#pragma endregion

#endif
