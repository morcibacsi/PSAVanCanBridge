#pragma once

#include <cstdint>

enum class ParkingAidStatus
{
    Undefined = 0,
    Wait = 0b101,
    OutOfService = 0b110,
    Active = 0b100,
};

enum class ParkingAidChannel
{
    None = 0,
    Left = 1,
    Right = 2,
    Both = 3,
};

enum class ParkingAidBarCount
{
    Zone1Near = 0b000,
    Zone2 = 0b001,
    Zone3 = 0b010,
    Zone4 = 0b011,
    Zone5 = 0b100,
    Zone6 = 0b101,
    Zone7 = 0b110,
    Zone8Far = 0b111,
};

enum class ParkingAidBeepDirection
{
    NoBeep = 0b00,
    Left = 0b01,
    Right = 0b10,
    LeftAndRight = 0b11,
};

enum class ParkingAidBeepDuration
{
    Duration1 = 0b00,
    Duration2 = 0b01,
    Duration3 = 0b10,
    Duration4 = 0b11,
};

enum class ParkingAidBeepLocation
{
    Rear = 0,
    Front = 1,
};

union ParkingAidStatusStruct {
    struct {
        uint8_t RearStatus         : 3;
        uint8_t FrontStatus        : 3;
        uint8_t SoundEnabled       : 1;
        uint8_t Channel            : 2;
        uint8_t RearDistance       : 3;
        uint8_t RearLeftDistance   : 3;
        uint8_t RearRightDistance  : 3;
        uint8_t FrontDistance      : 3;
        uint8_t FrontLeftDistance  : 3;
        uint8_t FrontRightDistance : 3;
        uint8_t BeepDirection      : 2;
        uint8_t BeepDuration       : 2;
        uint8_t BeepLocation       : 1;
        uint8_t BeepDelay          : 8; //0-62
        uint8_t VanParsingCanStart : 1;
        uint32_t unused            : 23;
    } data;
    uint64_t asNumeric;
};
