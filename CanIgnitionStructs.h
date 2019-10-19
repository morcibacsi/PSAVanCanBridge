// CanIgnitionStructs.h
#pragma once

#ifndef _CanIgnitionStructs_h
    #define _CanIgnitionStructs_h

#include "AbstractCanMessageSender.h"

// CANID: 036
const uint16_t CAN_ID_IGNITION = 0x036;

const int CAN_IGNITION_MODE_ON                = 0x01;
const int CAN_IGNITION_MODE_OFF               = 0x02;
const int CAN_IGNITION_MODE_ONCE_PER_IGNITION = 0x03;

// Read right to left in documentation
typedef struct {
    uint8_t                       : 1; // bit 0
    uint8_t                       : 1; // bit 1
    uint8_t                       : 1; // bit 2
    uint8_t                       : 1; // bit 3
    uint8_t                       : 1; // bit 4
    uint8_t                       : 1; // bit 5
    uint8_t                       : 1; // bit 6
    uint8_t economy_mode_active   : 1; // bit 7
} CanIgnitionByte3Struct;

typedef struct {
    uint8_t dashboard_brightness        : 4; // bit 0-3
    uint8_t disable_dark_mode           : 1; // bit 4
    uint8_t dashboard_lighting_enabled  : 1; // bit 5
    uint8_t                             : 1; // bit 6
    uint8_t                             : 1; // bit 7
} CanIgnitionByte4Struct;

typedef struct {
    uint8_t ignition_mode     : 3; // bit 0-2
    uint8_t                   : 1; // bit 3
    uint8_t                   : 1; // bit 4
    uint8_t                   : 1; // bit 5
    uint8_t                   : 1; // bit 6
    uint8_t                   : 1; // bit 7
} CanIgnitionByte5Struct;

// Read left to right in documentation
typedef struct CanIgnitionStruct {
    uint8_t Field1;
    uint8_t Field2;
    CanIgnitionByte3Struct Field3;
    CanIgnitionByte4Struct Brightness;
    CanIgnitionByte5Struct Ignition;
    uint8_t Field6;
    uint8_t Field7;
    uint8_t Field8;
};

typedef union CanIgnitionPacket {
    CanIgnitionStruct data;
    uint8_t CanIgnitionPacket[sizeof(CanIgnitionStruct)];
};

#pragma region Sender class
class CanIgnitionPacketSender
{
    AbstractCanMessageSender * canMessageSender;

public:
    CanIgnitionPacketSender(AbstractCanMessageSender * object)
    {
        canMessageSender = object;
    }

    void SendIgnition(uint8_t economy_mode_active, uint8_t brightness, uint8_t dashboard_lighting_enabled)
    {
        PacketGenerator<CanIgnitionPacket> generator1;

        generator1.packet.data.Field3.economy_mode_active = economy_mode_active;
        generator1.packet.data.Brightness.dashboard_lighting_enabled = dashboard_lighting_enabled;
        generator1.packet.data.Brightness.dashboard_brightness = brightness; //0-15
        generator1.packet.data.Ignition.ignition_mode = CAN_IGNITION_MODE_ON;

        unsigned char *serializedPacket1 = generator1.GetSerializedPacket();
        //unsigned char canMsg[8] = { 0x0E, 0x00, 0x05, 0x2F, 0x21, 0x80, 0x00, 0xA0 };
        canMessageSender->SendMessage(CAN_ID_IGNITION, 0, sizeof(CanIgnitionPacket), serializedPacket1);
    }

};
#pragma endregion

#endif
