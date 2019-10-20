// CanDash2Structs.h
#pragma once

#ifndef _CanDash2Structs_h
    #define _CanDash2Structs_h

#include "AbstractCanMessageSender.h"
#include "PacketGenerator.h"

// CANID: 128
const uint16_t CAN_ID_DASH2 = 0x128;

const uint8_t CAN_DASH_GEAR_P = 0x00;
const uint8_t CAN_DASH_GEAR_R = 0x10;
const uint8_t CAN_DASH_GEAR_N = 0x20;
const uint8_t CAN_DASH_GEAR_D = 0x30;
const uint8_t CAN_DASH_GEAR_6 = 0x40;
const uint8_t CAN_DASH_GEAR_5 = 0x50;
const uint8_t CAN_DASH_GEAR_4 = 0x60;
const uint8_t CAN_DASH_GEAR_3 = 0x70;
const uint8_t CAN_DASH_GEAR_2 = 0x80;
const uint8_t CAN_DASH_GEAR_1 = 0x90;

// Read right to left in documentation
typedef struct {
    uint8_t                       : 1; // bit 0
    uint8_t                       : 1; // bit 1
    uint8_t                       : 1; // bit 2
    uint8_t                       : 1; // bit 3
    uint8_t fuel_low_light        : 1; // bit 4
    uint8_t                       : 1; // bit 5
    uint8_t driver_seatbelt_light : 1; // bit 6
    uint8_t passenger_airbag_deactivated : 1; // bit 7
} CanDash2Byte1Struct;

typedef struct {
    uint8_t               : 1; // bit 0
    uint8_t               : 1; // bit 1
    uint8_t               : 1; // bit 2
    uint8_t               : 1; // bit 3
    uint8_t any_door_open : 1; // bit 4
    uint8_t               : 1; // bit 5
    uint8_t stop          : 1; // bit 6
    uint8_t warning       : 1; // bit 7
} CanDash2Byte2Struct;

typedef struct {
    uint8_t               : 1; // bit 0
    uint8_t               : 1; // bit 1
    uint8_t               : 1; // bit 2
    uint8_t esp_blink     : 1; // bit 3
    uint8_t               : 1; // bit 4
    uint8_t               : 1; // bit 5
    uint8_t               : 1; // bit 6
    uint8_t               : 1; // bit 7
} CanDash2Byte3Struct;

typedef struct {
    uint8_t                    : 1; // bit 0
    uint8_t left_indicator_on  : 1; // bit 1
    uint8_t right_indicator_on : 1; // bit 2
    uint8_t rear_fog_light_on  : 1; // bit 3
    uint8_t front_fog_light_on : 1; // bit 4
    uint8_t high_beam_on       : 1; // bit 5
    uint8_t low_beam_on        : 1; // bit 6
    uint8_t side_lights_on     : 1; // bit 7
} CanDash2Byte5Struct;

typedef struct {
    uint8_t                : 1; // bit 0
    uint8_t                : 1; // bit 1
    uint8_t                : 1; // bit 2
    uint8_t                : 1; // bit 3
    uint8_t                : 1; // bit 4
    uint8_t                : 1; // bit 5
    uint8_t                : 1; // bit 6
    uint8_t dash_enabled   : 1; // bit 7
} CanDash2Byte6Struct;

typedef struct {
    uint8_t blink          : 1; // bit 0
    uint8_t                : 1; // bit 1
    uint8_t                : 1; // bit 2
    uint8_t                : 1; // bit 3
    uint8_t gear           : 4; // bit 4-7
} CanDash2Byte7Struct;

typedef struct {
    uint8_t blink          : 1; // bit 0
    uint8_t                : 1; // bit 1
    uint8_t                : 1; // bit 2
    uint8_t                : 1; // bit 3
    uint8_t                : 1; // bit 4
    uint8_t gear_s         : 1; // bit 5
    uint8_t gear_snow      : 1; // bit 6 //if bit 
    uint8_t                : 1; // bit 7
} CanDash2Byte8Struct;

// Read left to right in documentation
typedef struct CanDash2Struct {
    CanDash2Byte1Struct Field1;
    CanDash2Byte2Struct Field2;
    CanDash2Byte3Struct Field3;
    uint8_t Field4;
    CanDash2Byte5Struct Field5;
    CanDash2Byte6Struct Field6;
    CanDash2Byte7Struct Field7;
    uint8_t Field8;
};

typedef union CanDash2Packet {
    CanDash2Struct data;
    uint8_t CanDash2Packet[sizeof(CanDash2Struct)];
};


class CanDash2PacketSender
{
    AbstractCanMessageSender * canMessageSender;

    public:
    CanDash2PacketSender(AbstractCanMessageSender * object)
    {
        canMessageSender = object;
    }

    void SendData(
        uint8_t driversSeatbeltLight,
        uint8_t sideLights,
        uint8_t lowBeam,
        uint8_t highBeam,
        uint8_t frontFog,
        uint8_t rearFog,
        uint8_t leftIndicator,
        uint8_t rightIndicator,
        uint8_t dashEnabled,
        uint8_t fuelLowLight,
        uint8_t passengerAirbagDeactivated)
    {
        PacketGenerator<CanDash2Packet> generator;
        generator.packet.data.Field1.driver_seatbelt_light = driversSeatbeltLight;
        generator.packet.data.Field1.fuel_low_light = fuelLowLight;
        generator.packet.data.Field1.passenger_airbag_deactivated = passengerAirbagDeactivated;

        generator.packet.data.Field5.side_lights_on = sideLights;
        generator.packet.data.Field5.low_beam_on = lowBeam;
        generator.packet.data.Field5.high_beam_on = highBeam;
        generator.packet.data.Field5.front_fog_light_on = frontFog;
        generator.packet.data.Field5.rear_fog_light_on = rearFog;
        generator.packet.data.Field5.left_indicator_on = leftIndicator;
        generator.packet.data.Field5.right_indicator_on = rightIndicator;

        generator.packet.data.Field6.dash_enabled = dashEnabled;

        unsigned char *serializedPacket = generator.GetSerializedPacket();
        canMessageSender->SendMessage(CAN_ID_DASH2, 0, sizeof(CanDash2Packet), serializedPacket);
    }
};
#endif
