// CanDash1Structs.h
#pragma once

#ifndef _CanDash1Structs_h
    #define _CanDash1Structs_h

#include "../AbstractCanMessageSender.h"
#include "../../Helpers/PacketGenerator.h"

// CANID: 0F6
const uint16_t CAN_ID_DASH1 = 0x0F6;

const uint8_t CAN_INDICATOR_LEFT  = 121; //y
const uint8_t CAN_INDICATOR_RIGHT = 120; //x

// Read right to left in documentation
typedef struct {
    uint8_t             : 1; // bit 0
    uint8_t             : 1; // bit 1
    uint8_t             : 1; // bit 2
    uint8_t ignition    : 1; // bit 3
    uint8_t             : 1; // bit 4
    uint8_t             : 1; // bit 5
    uint8_t             : 1; // bit 6
    uint8_t             : 1; // bit 7
} CanDash1Byte1Struct;

typedef struct {
    uint8_t bit0 : 1; // bit 0
    uint8_t bit1 : 1; // bit 1
    uint8_t bit2 : 1; // bit 2
    uint8_t bit3 : 1; // bit 3
    uint8_t bit4 : 1; // bit 4
    uint8_t bit5 : 1; // bit 5
    uint8_t bit6 : 1; // bit 6
    uint8_t economy_mode : 1; // bit 7
} CanDash1Byte2Struct;

typedef struct {
    uint8_t turn_left_light    : 1; // bit 0
    uint8_t turn_right_light   : 1; // bit 1
    uint8_t                    : 1; // bit 2
    uint8_t                    : 1; // bit 3
    uint8_t                    : 1; // bit 4
    uint8_t                    : 1; // bit 5
    uint8_t                    : 1; // bit 6
    uint8_t reverse_gear_light : 1; // bit 7
} CanDash1Byte8Struct;

// Read left to right in documentation
typedef struct CanDash1Struct {
    CanDash1Byte1Struct IgnitionField;
    uint8_t CoolantTemperature;
    uint8_t MileageByte1;
    uint8_t MileageByte2;
    uint8_t MileageByte3;
    uint8_t Field6;
    uint8_t ExternalTemperature;
    CanDash1Byte8Struct LightsStatus;
};

typedef union CanDash1Packet {
    CanDash1Struct data;
    uint8_t CanDash1Packet[sizeof(CanDash1Struct)];
};

int static CanGetCoolantTemperatureToDisplay(int temperatureToDisplay) {
    return (temperatureToDisplay + 39);
}

int static CanGetExternalTemperatureToDisplay(int temperatureToDisplay) {
    //actually it should be 39.5 not 40
    return (temperatureToDisplay + 40) * 2;
}

class CanDashIgnitionPacketSender
{
    AbstractCanMessageSender * canMessageSender;

    public:
    CanDashIgnitionPacketSender(AbstractCanMessageSender * object)
    {
        canMessageSender = object;
    }

    void SendIgnition(uint8_t ignition, int16_t coolantTemperature, int8_t externalTemperature, uint8_t mileageByte1, uint8_t mileageByte2, uint8_t mileageByte3, uint8_t isReverseEngaged)
    {
        if (!ignition)
        {
            coolantTemperature = 0;
        }

        PacketGenerator<CanDash1Packet> generator;
        generator.packet.data.IgnitionField.ignition = ignition;
        generator.packet.data.CoolantTemperature = CanGetCoolantTemperatureToDisplay(coolantTemperature);
        generator.packet.data.MileageByte1 = mileageByte1;
        generator.packet.data.MileageByte2 = mileageByte2;
        generator.packet.data.MileageByte3 = mileageByte3;
        generator.packet.data.ExternalTemperature = CanGetExternalTemperatureToDisplay(externalTemperature);
        generator.packet.data.LightsStatus.reverse_gear_light = isReverseEngaged;
        unsigned char *serializedPacket = generator.GetSerializedPacket();
        canMessageSender->SendMessage(CAN_ID_DASH1, 0, sizeof(CanDash1Packet), serializedPacket);
    }

};

#endif
