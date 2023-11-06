// CanStatusOfFunctionsStructs.h
#pragma once

#ifndef _CanStatusOfFunctionsStructs_h
    #define _CanStatusOfFunctionsStructs_h

// CANID: 2E1
const uint16_t CAN_ID_STATUS_OF_FUNCTIONS = 0x2E1;

// Read right to left in documentation
typedef struct {
    uint8_t AutomaticDoorLocking               : 1; // bit 0
    uint8_t AutomaticDoorLockingActivated      : 1; // bit 1
    uint8_t AutomaticHeadLampLighting          : 1; // bit 2 gearbox fault repair needed
    uint8_t AutomaticHeadLampLightingActivated : 1; // bit 3
    uint8_t PassengersAirbag                   : 1; // bit 4
    uint8_t PassengersAirbagActivated          : 1; // bit 5
    uint8_t ParkingAssistance                  : 1; // bit 6
    uint8_t ParkingAssistanceActivated         : 1; // bit 7
} CanStatusOfFunctionsByte0Struct;

typedef struct {
    uint8_t SportSuspensionMode             : 1; // bit 0
    uint8_t SportSuspensionModeDeactivated  : 1; // bit 1
    uint8_t AutomaticScreenWipe             : 1; // bit 2
    uint8_t AutomaticScreenWipeActivated    : 1; // bit 3
    uint8_t ESPSystem                       : 1; // bit 4
    uint8_t ESPSystemActivated              : 1; // bit 5
    uint8_t DoorUnlockingPerformed          : 1; // bit 6
    uint8_t DoorLockingPerformed            : 1; // bit 7
} CanStatusOfFunctionsByte1Struct;

typedef struct {
    uint8_t unknown0                 : 1; // bit 0
    uint8_t unknown1                 : 1; // bit 1
    uint8_t unknown2                 : 1; // bit 2
    uint8_t unknown3                 : 1; // bit 3
    uint8_t StopStartSystem          : 1; // bit 4 10
    uint8_t StopStartSystemActivated : 1; // bit 5 30
    uint8_t ChildSafety              : 1; // bit 6 40
    uint8_t ChildSafetyActivated     : 1; // bit 7 C0
} CanStatusOfFunctionsByte2Struct;


// Read left to right in documentation
struct CanStatusOfFunctionsStruct {
    CanStatusOfFunctionsByte0Struct FunctionsCategory0;
    CanStatusOfFunctionsByte1Struct FunctionsCategory1;
    CanStatusOfFunctionsByte2Struct FunctionsCategory2;
};

union CanStatusOfFunctionsPacket {
    CanStatusOfFunctionsStruct data;
    uint8_t CanStatusOfFunctionsPacket[sizeof(CanStatusOfFunctionsStruct)];
};

#endif
