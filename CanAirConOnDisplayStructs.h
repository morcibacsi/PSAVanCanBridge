// CanAirConOnDisplayStructs.h
#pragma once

#ifndef _CanAirConOnDisplayStructs_h
    #define _CanAirConOnDisplayStructs_h

// CANID: 1E3
const unsigned long CAN_ID_AIRCON_ON_DIPSLAY = 0x1E3;

const int CAN_AIRCON_DISPLAY_DIRECTION_AUTO        = 0x00;
const int CAN_AIRCON_DISPLAY_DIRECTION_DOWN        = 0x20;
const int CAN_AIRCON_DISPLAY_DIRECTION_FRONT       = 0x30;
const int CAN_AIRCON_DISPLAY_DIRECTION_UP          = 0x40;
const int CAN_AIRCON_DISPLAY_DIRECTION_FRONTDOWN   = 0x50;
const int CAN_AIRCON_DISPLAY_DIRECTION_UPDOWN      = 0x60;
const int CAN_AIRCON_DISPLAY_DIRECTION_FRONTUP     = 0x70;
const int CAN_AIRCON_DISPLAY_DIRECTION_FRONTUPDOWN = 0x80;

const uint8_t CAN_AIRCON_DISPLAY_WINDSHIELD  = 0x80;

// Read right to left in documentation
typedef struct {
    uint8_t separate_sides  : 1; // bit 0  If this flag is 0 then the temperature of the left side takes precedence
    uint8_t outside_air     : 1; // bit 1  Message: Forced intake of outside air
    uint8_t empty           : 1; // bit 2
    uint8_t auto_mode       : 1; // bit 3
    uint8_t unknown1        : 1; // bit 4  If this flag is 1 and bit 1 (outside_air) also set then the following message appears: Automatic check on air intake
    uint8_t off             : 1; // bit 5
    uint8_t ac_off          : 1; // bit 6
    uint8_t unknown2        : 1; // bit 7  If this flag is 1 and bit 1 (outside_air) also set then the following message appears: Cabin air recycling activated
} CanAirConOnDisplayByte1Struct;

typedef struct {
    uint8_t                 : 1; // bit 0
    uint8_t                 : 1; // bit 1
    uint8_t                 : 1; // bit 2
    uint8_t                 : 1; // bit 3
    uint8_t                 : 1; // bit 5
    uint8_t                 : 1; // bit 4
    uint8_t                 : 1; // bit 6
    uint8_t windshield      : 1; // bit 7
} CanAirConOnDisplayByte2Struct;

// Read left to right in documentation
typedef struct CanAirConOnDisplayStruct {
    CanAirConOnDisplayByte1Struct Modes;
    CanAirConOnDisplayByte2Struct WindShieldIf0x80;
    uint8_t TemperatureLeft;
    uint8_t TemperatureRight;
    uint8_t AirDirectionLeft;
    uint8_t AirDirectionRight;
    uint8_t Speed;
};

typedef union CanAirConOnDisplayPacket {
    CanAirConOnDisplayStruct data;
    uint8_t CanAirConOnDisplayPacket[sizeof(CanAirConOnDisplayStruct)];
};

uint8_t CanAirConToDisplayGetFanSpeed(uint8_t numberFrom0To8)
{
    // the display has a weird logic to display the fan speed
    // if 0 is sent then it will display 1
    // if 1 is sent then it will display 2
    // if 2 is sent then it will display 3
    //...
    // if 8 is sent then it will display 0

    uint8_t result = 0;
    if (0 <= numberFrom0To8 && numberFrom0To8 <= 8)
    {
        if (numberFrom0To8 == 0)
        {
            result = 8;
        }
        else
        {
            result = numberFrom0To8 - 1;
        }
    }
    else {
        if (numberFrom0To8 > 8)
        {
            result = 7;
        }
        if (numberFrom0To8 < 0)
        {
            result = 1;
        }
    }

    return result;
}

uint8_t CanAirConToDisplayGetTemperature(float temperature) {
    //0x00 : Min
    //0x01 - 0x05: 11 - 15
    //0x06 - 0x10: 15.5 - 20.5 (in 0.5C steps)
    //0x11 - 0x15: 21 - 25
    //0x16 : Max

    if (temperature < 11)
    {
        return 0x00;
    }
    if (temperature > 25)
    {
        return 0x16;
    }
    if (temperature <= 15)
    {
        return temperature - 10;
    }
    if (temperature >= 21)
    {
        return temperature - 4;
    }
    if (temperature == 16) return 0x07;
    if (temperature == 17) return 0x09;
    if (temperature == 18) return 0x0B;
    if (temperature == 19) return 0x0D;
    if (temperature == 20) return 0x0F;

    if (temperature == 15.5) return 0x06;
    if (temperature == 16.5) return 0x08;
    if (temperature == 17.5) return 0x0A;
    if (temperature == 18.5) return 0x0C;
    if (temperature == 19.5) return 0x0E;
    if (temperature == 19.5) return 0x10;
}

#pragma region Sender class
class CanAirConditionOnDisplayPacketSender
{
    AbstractCanMessageSender * canMessageSender;

public:
    CanAirConditionOnDisplayPacketSender(AbstractCanMessageSender * object)
    {
        canMessageSender = object;
    }

    void SendACDataToDisplay(float temperatureLeft, float temperatureRight , uint8_t direction, uint8_t autoMode, uint8_t acOff, uint8_t off, uint8_t windshield, uint8_t fanSpeed, uint8_t recyclingOn)
    {
        PacketGenerator<CanAirConOnDisplayPacket> generator;
        generator.packet.data.Modes.auto_mode = autoMode;
        generator.packet.data.Modes.ac_off = acOff;
        generator.packet.data.Modes.off = off;

        generator.packet.data.WindShieldIf0x80.windshield = windshield;

        generator.packet.data.Modes.separate_sides = 0;
        generator.packet.data.TemperatureLeft = CanAirConToDisplayGetTemperature(temperatureLeft);
        generator.packet.data.TemperatureRight = CanAirConToDisplayGetTemperature(temperatureRight);
        generator.packet.data.AirDirectionLeft = direction;
        generator.packet.data.AirDirectionRight = direction;
        generator.packet.data.Speed = CanAirConToDisplayGetFanSpeed(fanSpeed);

        if (recyclingOn == 1)
        {
            generator.packet.data.Modes.unknown2 = 1;
            generator.packet.data.Modes.outside_air = 1;
        }
        else
        {
            generator.packet.data.Modes.unknown2 = 0;
            generator.packet.data.Modes.outside_air = 0;
        }

        unsigned char *serializedPacket = generator.GetSerializedPacket();
        canMessageSender->SendMessage(CAN_ID_AIRCON_ON_DIPSLAY, 0, sizeof(CanAirConOnDisplayPacket), serializedPacket);
    }

};
#pragma endregion

#endif
