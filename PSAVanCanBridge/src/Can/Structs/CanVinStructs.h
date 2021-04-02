// CanVinStructs.h
#pragma once

#ifndef _CanVinStructs_h
    #define _CanVinStructs_h

#include "../AbstractCanMessageSender.h"
#include "../../Helpers/PacketGenerator.h"

// CANID: 336
const uint16_t CAN_ID_VIN_PART1 = 0x336;
// CANID: 3B6
const uint16_t CAN_ID_VIN_PART2 = 0x3B6;
// CANID: 2B6
const uint16_t CAN_ID_VIN_PART3 = 0x2B6;

// Read left to right in documentation

#pragma region ASCII coded first 3 letters of VIN

typedef struct CanVin1_3LettersStruct {
    uint8_t Letter1InAscii;
    uint8_t Letter2InAscii;
    uint8_t Letter3InAscii;
};

typedef union CanVin1Packet {
    CanVin1_3LettersStruct data;
    uint8_t CanVin1_3LettersMessagePacket[sizeof(CanVin1_3LettersStruct)];
};

#pragma endregion

#pragma region ASCII coded 4-9 letters of VIN

typedef struct CanVin4_9LettersStruct {
    uint8_t Letter4InAscii;
    uint8_t Letter5InAscii;
    uint8_t Letter6InAscii;
    uint8_t Letter7InAscii;
    uint8_t Letter8InAscii;
    uint8_t Letter9InAscii;
};

typedef union CanVin4_9LettersPacket {
    CanVin4_9LettersStruct data;
    uint8_t CanVin4_9LettersPacket[sizeof(CanVin4_9LettersStruct)];
};

#pragma endregion

#pragma region ASCII coded 10-17 letters (last 8) of VIN

typedef struct CanVin10_17LettersStruct {
    uint8_t Letter10InAscii;
    uint8_t Letter11InAscii;
    uint8_t Letter12InAscii;
    uint8_t Letter13InAscii;
    uint8_t Letter14InAscii;
    uint8_t Letter15InAscii;
    uint8_t Letter16InAscii;
    uint8_t Letter17InAscii;
};

typedef union CanVin10_17LettersPacket {
    CanVin10_17LettersStruct data;
    uint8_t CanVin10_17LettersPacket[sizeof(CanVin10_17LettersStruct)];
};

#pragma endregion

#pragma region Sender class
class CanVinPacketSender
{
    AbstractCanMessageSender * canMessageSender;

public:
    CanVinPacketSender(AbstractCanMessageSender * object)
    {
        canMessageSender = object;
    }

    void SendVinPart1(uint8_t vin[17])
    {
        PacketGenerator<CanVin1_3LettersStruct> generator1;
        generator1.packet.Letter1InAscii = vin[0];
        generator1.packet.Letter2InAscii = vin[1];
        generator1.packet.Letter3InAscii = vin[2];

        unsigned char *serializedPacket1 = generator1.GetSerializedPacket();
        canMessageSender->SendMessage(CAN_ID_VIN_PART1, 0, sizeof(CanVin1_3LettersStruct), serializedPacket1);
    }

    void SendVinPart2(uint8_t vin[17])
    {
        PacketGenerator<CanVin4_9LettersStruct> generator;
        generator.packet.Letter4InAscii = vin[3];
        generator.packet.Letter5InAscii = vin[4];
        generator.packet.Letter6InAscii = vin[5];
        generator.packet.Letter7InAscii = vin[6];
        generator.packet.Letter8InAscii = vin[7];
        generator.packet.Letter9InAscii = vin[8];

        unsigned char *serializedPacket = generator.GetSerializedPacket();
        canMessageSender->SendMessage(CAN_ID_VIN_PART2, 0, sizeof(CanVin4_9LettersStruct), serializedPacket);
    }

    void SendVinPart3(uint8_t vin[17])
    {
        PacketGenerator<CanVin10_17LettersStruct> generator;
        generator.packet.Letter10InAscii = vin[9];
        generator.packet.Letter11InAscii = vin[10];
        generator.packet.Letter12InAscii = vin[11];
        generator.packet.Letter13InAscii = vin[12];
        generator.packet.Letter14InAscii = vin[13];
        generator.packet.Letter15InAscii = vin[14];
        generator.packet.Letter16InAscii = vin[15];
        generator.packet.Letter17InAscii = vin[16];

        unsigned char *serializedPacket = generator.GetSerializedPacket();
        canMessageSender->SendMessage(CAN_ID_VIN_PART3, 0, sizeof(CanVin10_17LettersStruct), serializedPacket);
    }
};
#pragma endregion



#endif
