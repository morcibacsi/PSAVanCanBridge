// CanDash4Structs.h
#pragma once

#ifndef _CanDash4Structs_h
    #define _CanDash4Structs_h

#include "../AbstractCanMessageSender.h"
#include "../../Helpers/PacketGenerator.h"

// CANID: 161
const uint16_t CAN_ID_DASH4 = 0x161;

// Read left to right in documentation
struct CanDash4Struct {
    uint8_t Field1;
    uint8_t Field2;
    uint8_t OilTemperature;
    uint8_t FuelLevel;
    uint8_t Field5;
    uint8_t Field6;
    uint8_t Field7;
    uint8_t Field8;
};

union CanDash4Packet {
    CanDash4Struct data;
    uint8_t CanDash4Packet[sizeof(CanDash4Struct)];
};

class CanDash4PacketSender
{
    AbstractCanMessageSender * canMessageSender;

    uint8_t GetOilTemperatureToDisplay(int8_t temperature)
    {
        // The oil temperature display is not a precise display the following chart shows where the display changes
        // The left value indicates when the display changes when the temperature goes from low to high direction
        // The right value indicates when the display changes when the temperature goes from high to low direction
        /*
            0xAC    0xAF    display fills the 7th block (150 C)
            0x9E    0xA1    display fills the 6th block
            0x90    0x93    display fills the 5th block
            0x74    0x77    display fills the 4th block (100 C)
            0x6B    0x6E    display fills the 3rd block
            0x61    0x64    display fills the 2nd block
            0x57    0x5A    display fills the 1st block (leaves 50 C)
        */
        return temperature + 40;
    }

    int8_t GetOilTemperatureToFromCanByte(uint8_t canByte)
    {
        return canByte - 40;
    }

    public:
    CanDash4PacketSender(AbstractCanMessageSender * object)
    {
        canMessageSender = object;
    }

    void SendData(uint8_t fuelLevel, int8_t oilTemperature)
    {
        PacketGenerator<CanDash4Packet> generator;
        generator.packet.data.FuelLevel = fuelLevel;
        generator.packet.data.OilTemperature = GetOilTemperatureToDisplay(oilTemperature);

        unsigned char *serializedPacket = generator.GetSerializedPacket();
        canMessageSender->SendMessage(CAN_ID_DASH4, 0, sizeof(CanDash4Packet), serializedPacket);
    }
};
#endif
