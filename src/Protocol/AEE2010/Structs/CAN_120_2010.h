#pragma once

#ifndef _2010_120_h
    #define _2010_120_h

#include <stdint.h>

// CANID: 120
const uint16_t CAN2010_ID_120 = 0x120;
const uint8_t CAN2010_ID_120_LEN = 8;

const uint8_t MENU_NOT_AVAILABLE_AND_ACTIVATION_IMPOSSIBLE_2010 = 0;
const uint8_t MENU_NOT_AVAILABLE_ACTIVATION_POSSIBLE_2010       = 1;
const uint8_t MENU_AVAILABLE_AND_ACTIVATION_IMPOSSIBLE_2010     = 2;
const uint8_t MENU_AVAILABLE_AND_ACTIVATION_POSSIBLE_2010       = 3;

const uint8_t TOTAL_BLOCK_NUMBER_INVALID_2010 = 0;
const uint8_t TOTAL_BLOCK_NUMBER_1_2010       = 1;
const uint8_t TOTAL_BLOCK_NUMBER_2_2010       = 2;
const uint8_t TOTAL_BLOCK_NUMBER_3_2010       = 3;

const uint8_t BLOCK_NUMBER_INVALID_2010 = 0;
const uint8_t BLOCK_NUMBER_1_2010       = 1;
const uint8_t BLOCK_NUMBER_2_2010       = 2;
const uint8_t BLOCK_NUMBER_3_2010       = 3;

union CAN_120_2010_Byte1Struct {
    struct {
        uint8_t                        : 2; // bit 0-1
        uint8_t menu_available         : 2; // bit 2-3
        uint8_t total_number_of_blocks : 2; // bit 4-5
        uint8_t block_number           : 2; // bit 6-7
    } data;
    uint8_t asByte;
};

// Read right to left in documentation
union CAN_120_2010_Byte2Struct {
    struct
    {
        uint8_t unknown0                    : 1; // bit 0
        uint8_t HighSpeedCheckTyrePressures : 1; // bit 1 High speed, check tyre pressures correct
        uint8_t unknown2                    : 1; // bit 2
        uint8_t unknown3                    : 1; // bit 3
        uint8_t FuelTankAccessLockError     : 1; // bit 4 Fuel tank access not properly locked
        uint8_t RearScreenOpen              : 1; // bit 5 Rear srceen open
        uint8_t BonnetOpen                  : 1; // bit 6 Bonnet open
        uint8_t BootOpen                    : 1; // bit 7 Boot open
    } data;
    uint8_t asByte;
};

union CAN_120_2010_Byte3Struct {
    struct
    {
        uint8_t unknown0                    : 1; // bit 0
        uint8_t DirectionalHeadLampsFaulty  : 1; // bit 1 Directional headlamps faulty
        uint8_t BonnetOpen                  : 1; // bit 2 Bonnet open
        uint8_t AdjustableWingFault         : 1; // bit 3 Adjustable wing fault. speed restricted. see handbook
        uint8_t ParkingBrakeFaulty          : 1; // bit 4 Parking brake faulty
        uint8_t unknown5                    : 1; // bit 5
        uint8_t unknown6                    : 1; // bit 6
        uint8_t unknown7                    : 1; // bit 7
    } data;
    uint8_t asByte;
};

union CAN_120_2010_Byte4Struct {
    struct
    {
        uint8_t unknown0                 : 1; // bit 0
        uint8_t unknown1                 : 1; // bit 1
        uint8_t GearboxFaultRepairNeeded : 1; // bit 2 Gearbox fault repair needed
        uint8_t unknown3                 : 1; // bit 3
        uint8_t unknown4                 : 1; // bit 4
        uint8_t unknown5                 : 1; // bit 5
        uint8_t unknown6                 : 1; // bit 6
        uint8_t unknown7                 : 1; // bit 7
    } data;
    uint8_t asByte;
};

union CAN_120_2010_Byte5Struct {
    struct
    {
        uint8_t unknown0                 : 1; // bit 0
        uint8_t SuspensionFaulty         : 1; // bit 1 Suspension faulty max speed 90 kmh
        uint8_t EngineFaultRepairNeeded  : 1; // bit 2 Engine fault repair needed
        uint8_t unknown3                 : 1; // bit 3
        uint8_t TyrePressuresTooLow      : 1; // bit 4 Tyre pressure(s) too low
        uint8_t unknown5                 : 1; // bit 5
        uint8_t AntiWanderDeviceFaulty   : 1; // bit 6 Anti-wander system lane-crossing warning device faulty
        uint8_t unknown7                 : 1; // bit 7
    } data;
    uint8_t asByte;
};

union CAN_120_2010_Byte6Struct {
    struct
    {
        uint8_t RearLHTyreNotMonitored  : 1; // bit 0
        uint8_t RearRHTyreNotMonitored  : 1; // bit 1
        uint8_t FrontRHTyreNotMonitored : 1; // bit 2
        uint8_t FrontLHTyreNotMonitored : 1; // bit 3
        uint8_t unknown4                : 1; // bit 4
        uint8_t unknown5                : 1; // bit 5
        uint8_t unknown6                : 1; // bit 6
        uint8_t unknown7                : 1; // bit 7
    } data;
    uint8_t asByte;
};

union CAN_120_2010_Byte7Struct {
    struct
    {
        uint8_t unknown0                  : 1; // bit 0
        uint8_t unknown1                  : 1; // bit 1
        uint8_t EngineFaultStopTheVehicle : 1; // bit 2 Engine fault stop the vehicle
        uint8_t unknown3                  : 1; // bit 3
        uint8_t unknown4                  : 1; // bit 4
        uint8_t unknown5                  : 1; // bit 5
        uint8_t PowerSteeringFaulty       : 1; // bit 6 Power steering faulty
        uint8_t SuspensionFaulty          : 1; // bit 7 Suspension faulty
    } data;
    uint8_t asByte;
};


struct CMB2010_120Struct {
    CAN_120_2010_Byte1Struct Status;
    uint8_t Field2;
    uint8_t Field3;
    uint8_t Field4;
    uint8_t Field5;
    uint8_t Field6;
    uint8_t Field7;
    uint8_t Field8;
};

union CMB2010_120Packet {
    CMB2010_120Struct data;
    uint8_t CMB2004_120Packet[sizeof(CMB2010_120Struct)];
};
#endif
