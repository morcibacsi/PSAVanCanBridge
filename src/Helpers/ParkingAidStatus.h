#pragma once

#include <cstdint>

enum class ParkingAidStatus
{
    Disabled = 0,
    Active = 0b100,
};

enum class ParkingAidLocation
{
    Rear = 0,
    Front = 1,
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
        uint8_t RearStatus;
        uint8_t SoundEnabled;
        uint8_t Location;
        uint8_t Channel;
        uint8_t RearDistance;
        uint8_t RearLeftDistance;
        uint8_t RearRightDistance;
        uint8_t FrontDistance;
        uint8_t FrontLeftDistance;
        uint8_t FrontRightDistance;
        uint8_t BeepDirection;
        uint8_t BeepDuration;
        uint8_t BeepLocation;
        uint8_t BeepDelay; //0-62
    } data;
    uint8_t asByte;
};
