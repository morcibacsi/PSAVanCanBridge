// CanWarningLogStructs.h
#pragma once

#ifndef _CanWarningLogStructs_h
    #define _CanWarningLogStructs_h

// CANID: 120
const uint16_t CAN_ID_WARNING_LOG = 0x120;

const uint8_t CAN_WARNING_LOG_ENABLE = 0xFF;
const uint8_t CAN_WARNING_LOG_DISABLE = 0x00;

// Read right to left in documentation
typedef struct {
    uint8_t unknown0                    : 1; // bit 0
    uint8_t HighSpeedCheckTyrePressures : 1; // bit 1 High speed, check tyre pressures correct
    uint8_t unknown2                    : 1; // bit 2
    uint8_t unknown3                    : 1; // bit 3
    uint8_t FuelTankAccessLockError     : 1; // bit 4 Fuel tank access not properly locked
    uint8_t RearScreenOpen              : 1; // bit 5 Rear srceen open
    uint8_t BonnetOpen                  : 1; // bit 6 Bonnet open
    uint8_t BootOpen                    : 1; // bit 7 Boot open
} CanWarningLogByte1Struct;

typedef struct {
    uint8_t unknown0                    : 1; // bit 0
    uint8_t DirectionalHeadLampsFaulty  : 1; // bit 1 Directional headlamps faulty
    uint8_t BonnetOpen                  : 1; // bit 2 Bonnet open
    uint8_t AdjustableWingFault         : 1; // bit 3 Adjustable wing fault. speed restricted. see handbook
    uint8_t ParkingBrakeFaulty          : 1; // bit 4 Parking brake faulty
    uint8_t unknown5                    : 1; // bit 5
    uint8_t unknown6                    : 1; // bit 6
    uint8_t unknown7                    : 1; // bit 7
} CanWarningLogByte2Struct;

typedef struct {
    uint8_t unknown0                 : 1; // bit 0
    uint8_t unknown1                 : 1; // bit 1
    uint8_t GearboxFaultRepairNeeded : 1; // bit 2 Gearbox fault repair needed
    uint8_t unknown3                 : 1; // bit 3
    uint8_t unknown4                 : 1; // bit 4
    uint8_t unknown5                 : 1; // bit 5
    uint8_t unknown6                 : 1; // bit 6
    uint8_t unknown7                 : 1; // bit 7
} CanWarningLogByte3Struct;

typedef struct {
    uint8_t unknown0                 : 1; // bit 0
    uint8_t SuspensionFaulty         : 1; // bit 1 Suspension faulty max speed 90 kmh
    uint8_t EngineFaultRepairNeeded  : 1; // bit 2 Engine fault repair needed
    uint8_t unknown3                 : 1; // bit 3
    uint8_t TyrePressuresTooLow      : 1; // bit 4 Tyre pressure(s) too low
    uint8_t unknown5                 : 1; // bit 5
    uint8_t AntiWanderDeviceFaulty   : 1; // bit 6 Anti-wander system lane-crossing warning device faulty
    uint8_t unknown7                 : 1; // bit 7
} CanWarningLogByte4Struct;

typedef struct {
    uint8_t RearLHTyreNotMonitored  : 1; // bit 0
    uint8_t RearRHTyreNotMonitored  : 1; // bit 1
    uint8_t FrontRHTyreNotMonitored : 1; // bit 2
    uint8_t FrontLHTyreNotMonitored : 1; // bit 3
    uint8_t unknown4                : 1; // bit 4
    uint8_t unknown5                : 1; // bit 5
    uint8_t unknown6                : 1; // bit 6
    uint8_t unknown7                : 1; // bit 7
} CanWarningLogByte5Struct;

typedef struct {
    uint8_t unknown0                  : 1; // bit 0
    uint8_t unknown1                  : 1; // bit 1
    uint8_t EngineFaultStopTheVehicle : 1; // bit 2 Engine fault stop the vehicle
    uint8_t unknown3                  : 1; // bit 3
    uint8_t unknown4                  : 1; // bit 4
    uint8_t unknown5                  : 1; // bit 5
    uint8_t PowerSteeringFaulty       : 1; // bit 6 Power steering faulty
    uint8_t SuspensionFaulty          : 1; // bit 7 Suspension faulty
} CanWarningLogByte6Struct;

// Read left to right in documentation
struct CanWarningLogStruct {
    uint8_t EnableLog;
    CanWarningLogByte1Struct WarningCategory1;
    CanWarningLogByte2Struct WarningCategory2;
    CanWarningLogByte3Struct WarningCategory3;
    CanWarningLogByte4Struct WarningCategory4;
    CanWarningLogByte5Struct TyreWarning;
    CanWarningLogByte6Struct WarningCategory6;
    uint8_t Field7;
};

union CanWarningLogPacket {
    CanWarningLogStruct data;
    uint8_t CanWarningLogPacket[sizeof(CanWarningLogStruct)];
};

#endif
