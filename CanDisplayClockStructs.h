// CanDisplayClockStructs.h
#pragma once

#ifndef _CanDisplayClockStructs_h
    #define _CanDisplayClockStructs_h


// CANID: 276 (Peugeot 301 display)
const uint16_t CAN_ID_DISPLAY_CLOCK = 0x276;

// Read left to right in documentation
typedef struct CanDisplayClockStruct {
    uint8_t Year;
    uint8_t Month;
    uint8_t Day;
    uint8_t Hour;
    uint8_t Minute;
};

typedef union CanDisplayClockPacket {
    CanDisplayClockStruct data;
    uint8_t CanDisplayClockPacket[sizeof(CanDisplayClockStruct)];
};

uint8_t GetYearToSendToDisplay(int year)
{
    return year - 2000;
}

#pragma region Sender class
class CanDisplayClockPacketSender
{
    AbstractCanMessageSender * canMessageSender;

public:
    CanDisplayClockPacketSender(AbstractCanMessageSender * object)
    {
        canMessageSender = object;
    }

    void Send(unsigned int year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute)
    {
        PacketGenerator<CanDisplayClockPacket> generator;

        generator.packet.data.Year = GetYearToSendToDisplay(year);
        generator.packet.data.Month = month;
        generator.packet.data.Day = day;
        generator.packet.data.Hour = hour;
        generator.packet.data.Minute = minute;

        unsigned char *serializedPacket = generator.GetSerializedPacket();
        canMessageSender->SendMessage(CAN_ID_DISPLAY_CLOCK, 0, sizeof(CanDisplayClockPacket), serializedPacket);
    }
};
#pragma endregion

#endif
