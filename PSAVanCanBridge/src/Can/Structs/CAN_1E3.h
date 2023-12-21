#pragma once

#ifndef _CAN_1E3_h
    #define _CAN_1E3_h

// CANID: 1E3
const uint16_t CAN_ID_AIRCON_ON_DIPSLAY = 0x1E3;

const uint8_t CAN_AIRCON_DISPLAY_DIRECTION_AUTO        = 0x00;
const uint8_t CAN_AIRCON_DISPLAY_DIRECTION_DOWN        = 0x20;
const uint8_t CAN_AIRCON_DISPLAY_DIRECTION_FRONT       = 0x30;
const uint8_t CAN_AIRCON_DISPLAY_DIRECTION_UP          = 0x40;
const uint8_t CAN_AIRCON_DISPLAY_DIRECTION_FRONTDOWN   = 0x50;
const uint8_t CAN_AIRCON_DISPLAY_DIRECTION_UPDOWN      = 0x60;
const uint8_t CAN_AIRCON_DISPLAY_DIRECTION_FRONTUP     = 0x70;
const uint8_t CAN_AIRCON_DISPLAY_DIRECTION_FRONTUPDOWN = 0x80;

const uint8_t CAN_AIRCON_DISPLAY_WINDSHIELD  = 0x80;

// Read right to left in documentation
union CanAirConOnDisplayByte1Struct {
    struct {
        uint8_t separate_sides      : 1; // bit 0  If this flag is 0 then the temperature of the left side takes precedence
        uint8_t recycling_on_pushed : 1; // bit 1  Message: Forced intake of outside air
        uint8_t auto_speed          : 1; // bit 2
        uint8_t auto_mode           : 1; // bit 3
        uint8_t airflow_auto        : 1; // bit 4  If this flag is 1 and bit 1 (outside_air) also set then the following message appears: Automatic check on air intake
        uint8_t off                 : 1; // bit 5
        uint8_t ac_compressor_off   : 1; // bit 6
        uint8_t recycling_on        : 1; // bit 7  If this flag is 1 and bit 1 (outside_air) also set then the following message appears: Cabin air recycling activated
    } data;
    uint8_t asByte;
};

union CanAirConOnDisplayByte2Struct {
    struct {
        uint8_t                        : 3; // bit 0-2
        uint8_t rear_window_heating_on : 1; // bit 3
        uint8_t center_temperature     : 3; // bit 4-6
        uint8_t windshield             : 1; // bit 7
    } data;
    uint8_t asByte;
};

// Read left to right in documentation
struct CanAirConOnDisplayStruct {
    CanAirConOnDisplayByte1Struct Modes;
    CanAirConOnDisplayByte2Struct Field2;
    uint8_t TemperatureLeft;
    uint8_t TemperatureRight;
    uint8_t AirDirectionLeft;
    uint8_t AirDirectionRight;
    uint8_t Speed; //0-7 = 1-8 0xF = 0
};

union CanAirConOnDisplayPacket {
    CanAirConOnDisplayStruct data;
    uint8_t CanAirConOnDisplayPacket[sizeof(CanAirConOnDisplayStruct)];
};

uint8_t static CanAirConToDisplayGetFanSpeed(uint8_t numberFrom0To8)
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

uint8_t static CanAirConToDisplayGetTemperature(float temperature) {
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

    return 0x16;
}

#endif
