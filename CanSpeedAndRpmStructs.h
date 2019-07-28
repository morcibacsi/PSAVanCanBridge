// CanSpeedAndRpmStructs.h
#pragma once

#ifndef _CanSpeedAndRpmStructs_h
    #define _CanSpeedAndRpmStructs_h

// CANID: 0B6
const unsigned long CAN_ID_SPEED_AND_RPM = 0x0B6;

typedef struct {
    unsigned short Data : 16;
} CanRpmStruct;

typedef struct {
    unsigned short Data : 16;
} CanSpeedStruct;

// Read left to right in documentation
typedef struct CanSpeedAndRpmStruct {
    CanRpmStruct Rpm;
    CanSpeedStruct Speed;
    CanSpeedStruct Odometer;
    uint8_t FuelConsumptionCounter;
    uint8_t Field4;
};

typedef union CanSpeedAndRpmPacket {
    CanSpeedAndRpmStruct data;
    uint8_t CanSpeedAndRpmPacket[sizeof(CanSpeedAndRpmStruct)];
};

#pragma region Sender class
class CanSpeedAndRpmPacketSender
{
    AbstractCanMessageSender * canMessageSender;

    unsigned int SwapLoByteWithHiByte(unsigned int input)
    {
        // swap low order byte with high order byte
        return ((input & 0xff) << 8) | ((input >> 8) & 0xff);
    }

public:
    CanSpeedAndRpmPacketSender(AbstractCanMessageSender * object)
    {
        canMessageSender = object;
    }

    void Send(uint8_t speed, uint16_t rpm)
    {
        PacketGenerator<CanSpeedAndRpmPacket> generator;

        generator.packet.data.Rpm.Data = SwapLoByteWithHiByte(rpm * 8);
        generator.packet.data.Speed.Data = SwapLoByteWithHiByte(speed * 100);
        generator.packet.data.Odometer.Data = 0x24B9;
        generator.packet.data.FuelConsumptionCounter = 0x89;
        generator.packet.data.Field4 = 0xD0;

        unsigned char *serializedPacket = generator.GetSerializedPacket();
        canMessageSender->SendMessage(CAN_ID_SPEED_AND_RPM, 0, sizeof(CanSpeedAndRpmPacket), serializedPacket);
    }
};
#pragma endregion

#endif
