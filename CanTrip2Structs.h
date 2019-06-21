// CanTrip2Structs.h
#pragma once

#ifndef _CanTrip2Structs_h
    #define _CanTrip2Structs_h

// CANID: 261
const unsigned long CAN_ID_TRIP2 = 0x261;

// Read left to right in documentation
typedef struct CanTrip2Struct {
    uint8_t AverageSpeed;
    uint8_t MileageAfterResetByte1;
    uint8_t MileageAfterResetByte2;
    uint8_t LitersPer100KmByte1;
    uint8_t LitersPer100KmByte2;
    uint8_t Field6;
    uint8_t Field7;
};

typedef union CanTrip2Packet {
    CanTrip2Struct data;
    uint8_t CanTrip2Packet[sizeof(CanTrip2Struct)];
};

#pragma region Sender class
class CanTrip2Sender
{
    AbstractCanMessageSender * canMessageSender;

public:
    CanTrip2Sender(AbstractCanMessageSender * object)
    {
        canMessageSender = object;
    }

    void SendTripInfo(int mileageAfterReset, uint8_t lper100km, uint8_t averageSpeed)
    {
        PacketGenerator<CanTrip2Packet> generator;

        IntValueOnTwoBytes.value = mileageAfterReset;
        generator.packet.data.MileageAfterResetByte1 = IntValueOnTwoBytes.bytes[1];
        generator.packet.data.MileageAfterResetByte2 = IntValueOnTwoBytes.bytes[0];

        IntValueOnTwoBytes.value = lper100km;
        generator.packet.data.LitersPer100KmByte1 = IntValueOnTwoBytes.bytes[1];
        generator.packet.data.LitersPer100KmByte2 = IntValueOnTwoBytes.bytes[0];

        generator.packet.data.AverageSpeed = averageSpeed;

        unsigned char *serializedPacket = generator.GetSerializedPacket();
        canMessageSender->SendMessage(CAN_ID_TRIP2, 0, sizeof(CanTrip2Packet), serializedPacket);
    }

};
#pragma endregion

#endif
