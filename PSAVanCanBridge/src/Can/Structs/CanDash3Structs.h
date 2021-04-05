// CanDash3Structs.h
#pragma once

#ifndef _CanDash3Structs_h
    #define _CanDash3Structs_h

#include "../AbstractCanMessageSender.h"
#include "../../Helpers/PacketGenerator.h"

// CANID: 168
const uint16_t CAN_ID_DASH3 = 0x168;

// Read right to left in documentation
typedef struct {
    uint8_t                       : 1; // bit 0
    uint8_t                       : 1; // bit 1
    uint8_t handbrake             : 1; // bit 2
    uint8_t                       : 1; // bit 3
    uint8_t                       : 1; // bit 4
    uint8_t                       : 1; // bit 5
    uint8_t oil_temp_max          : 1; // bit 6
    uint8_t water_temp_max        : 1; // bit 7
} CanDash3Byte1Struct;

typedef struct {
    uint8_t                    : 1; // bit 0
    uint8_t mil                : 1; // bit 1
    uint8_t                    : 1; // bit 2
    uint8_t                    : 1; // bit 3
    uint8_t esp                : 1; // bit 4
    uint8_t abs                : 1; // bit 5
    uint8_t                    : 1; // bit 6
    uint8_t handbrake          : 1; // bit 7
} CanDash3Byte4Struct;

typedef struct {
    uint8_t                    : 1; // bit 0
    uint8_t                    : 1; // bit 1
    uint8_t                    : 1; // bit 2
    uint8_t                    : 1; // bit 3
    uint8_t mil_blink          : 1; // bit 4
    uint8_t driver_airbag      : 1; // bit 5
    uint8_t                    : 1; // bit 6
    uint8_t                    : 1; // bit 7
} CanDash3Byte5Struct;


// Read left to right in documentation
struct CanDash3Struct {
    CanDash3Byte1Struct Field1;
    uint8_t Field2;
    uint8_t Field3;
    CanDash3Byte4Struct Field4;
    CanDash3Byte5Struct Field5;
    uint8_t Field6;
    uint8_t Field7;
    uint8_t Field8;
};

union CanDash3Packet {
    CanDash3Struct data;
    uint8_t CanDash3Packet[sizeof(CanDash3Struct)];
};

class CanDash3PacketSender
{
    AbstractCanMessageSender * canMessageSender;

    public:
    CanDash3PacketSender(AbstractCanMessageSender * object)
    {
        canMessageSender = object;
    }

    void SendData(uint8_t handbrake, uint8_t mil, uint8_t abs, uint8_t esp, uint8_t airbag)
    {
        PacketGenerator<CanDash3Packet> generator;
        generator.packet.data.Field1.handbrake = handbrake;

        generator.packet.data.Field4.handbrake = handbrake;
        generator.packet.data.Field4.mil = mil;
        generator.packet.data.Field4.abs = abs;
        generator.packet.data.Field4.esp = esp;

        generator.packet.data.Field5.driver_airbag = airbag;

        unsigned char *serializedPacket = generator.GetSerializedPacket();
        canMessageSender->SendMessage(CAN_ID_DASH3, 0, sizeof(CanDash3Packet), serializedPacket);
    }
};
#endif
