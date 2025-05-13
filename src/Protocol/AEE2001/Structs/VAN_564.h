#pragma once

#ifndef _VAN_564_h
    #define _VAN_564_h

// VANID: 564
const uint16_t VAN_ID_CARSTATUS = 0x564;

// Read right to left in documentation
typedef struct {
    unsigned short data : 16;
} VanTripDistanceStruct;

typedef struct {
    uint8_t FuelFlap    : 1; // bit 0
    uint8_t Sunroof     : 1; // bit 1
    uint8_t Hood        : 1; // bit 2
    uint8_t BootLid     : 1; // bit 3
    uint8_t RearLeft    : 1; // bit 4
    uint8_t RearRight   : 1; // bit 5
    uint8_t FrontLeft   : 1; // bit 6
    uint8_t FrontRight  : 1; // bit 7
} VanDoorStatus;

typedef struct {
    uint8_t TripButton  : 1; // bit 0
    uint8_t Unknown1    : 1; // bit 1
    uint8_t Unknown2    : 1; // bit 2
    uint8_t Unknown3    : 1; // bit 3
    uint8_t Unknown4    : 1; // bit 4
    uint8_t Unknown5    : 1; // bit 5
    uint8_t Unknown6    : 1; // bit 6
    uint8_t Unknown7    : 1; // bit 7
} VanCarStatusWithTripComputerField10ByteStruct;

// Read left to right in documentation
struct VanCarStatusWithTripComputerStruct {
    uint8_t Header;
    uint8_t Field1;
    uint8_t Field2;
    uint8_t Field3;
    uint8_t Field4;
    uint8_t Field5;
    uint8_t Field6;
    VanDoorStatus Doors;
    uint8_t Field8;
    uint8_t Field9;
    VanCarStatusWithTripComputerField10ByteStruct Field10;
    uint8_t AvgCourseSpeed;
    uint8_t AvgTravelSpeed;
    uint8_t Field13;
    uint8_t Trip1Distance1;
    uint8_t Trip1Distance2;
    //Should be divided by 10
    uint8_t Trip1FuelConsumption1;
    uint8_t Trip1FuelConsumption2;
    uint8_t Trip2Distance1;
    uint8_t Trip2Distance2;
    //Should be divided by 10
    uint8_t Trip2FuelConsumption1;
    uint8_t Trip2FuelConsumption2;
    uint8_t FuelConsumption1;
    uint8_t FuelConsumption2;
    uint8_t FuelLeftToPumpInKm1;
    uint8_t FuelLeftToPumpInKm2;
    uint8_t Footer;
};

union VanCarStatusWithTripComputerPacket {
    VanCarStatusWithTripComputerStruct data;
    uint8_t VanCarStatusWithTripComputerPacket[sizeof(VanCarStatusWithTripComputerStruct)];
};

#endif
