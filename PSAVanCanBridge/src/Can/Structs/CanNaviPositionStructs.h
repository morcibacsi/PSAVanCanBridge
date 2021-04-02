// CanNaviPositionStructs.h
#pragma once

#ifndef _CanNaviPositionStructs_h
    #define _CanNaviPositionStructs_h

#include "../AbstractCanMessageSender.h"
#include "../../Helpers/PacketGenerator.h"

const uint8_t CAN_ID_NAVI_POS_INTERVAL = 100;

// CANID: 0E6
const uint16_t CAN_ID_NAVI_POS = 0x0E6;

typedef struct {
    uint8_t fault_lamp_abr : 1; // bit 0
    uint8_t brake_fluid_   : 1; // bit 1
    uint8_t brake_pad_worn : 1; // bit 2
    uint8_t sliding_active : 2; // bit 3-4
    uint8_t is_rotating    : 1; // bit 5
    uint8_t abs_fault      : 1; // bit 6
    uint8_t fault_lamp_ref : 1; // bit 7
} CanAbsStatus;

typedef union {
    CanAbsStatus status;
    uint8_t asByte;
} CanAbsStatusUnion;

typedef struct {
    uint8_t wheel_counter : 15; // bit 0-6
    uint8_t error_status  : 1; // bit 7
} CanWheelCounterAndStatus;

typedef union {
    CanAbsStatus status;
    uint16_t asRawValue;
} CanWheelCounterAndStatusUnion;

// Read left to right in documentation
typedef struct CanNaviPositionStruct {
    CanAbsStatusUnion AbsStatus;
    CanWheelCounterAndStatusUnion RearLeftAbsStatus;
    CanWheelCounterAndStatusUnion RearRightAbsStatus;
};

typedef union CanNaviPositionPacket {
    CanNaviPositionStruct data;
    uint8_t CanNaviPositionPacket[sizeof(CanNaviPositionStruct)];
};

#pragma region Sender class
class CanNaviPositionPacketSender
{
    AbstractCanMessageSender* canMessageSender;

    unsigned int SwapLoByteWithHiByte(unsigned int input)
    {
        // swap low order byte with high order byte
        return ((input & 0xff) << 8) | ((input >> 8) & 0xff);
    }

public:
    CanNaviPositionPacketSender(AbstractCanMessageSender* object)
    {
        canMessageSender = object;
    }

    void Send(uint16_t rightWheelValue, uint16_t leftWheelValue)
    {
        PacketGenerator<CanNaviPositionPacket> generator;

        generator.packet.data.AbsStatus.asByte = 0;
        generator.packet.data.RearLeftAbsStatus.asRawValue = leftWheelValue;
        generator.packet.data.RearRightAbsStatus.asRawValue = rightWheelValue;

        unsigned char* serializedPacket = generator.GetSerializedPacket();
        canMessageSender->SendMessage(CAN_ID_NAVI_POS, 0, sizeof(CanNaviPositionPacket), serializedPacket);
    }
};
#pragma endregion


#endif
