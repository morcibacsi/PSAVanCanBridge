// CanDefineVehicleParameters.h
#pragma once

#ifndef _CanDefineVehicleParameters_h
    #define _CanDefineVehicleParameters_h

// CANID: 361
const uint16_t CAN_ID_DEFINE_VEHICLE_PARAMETERS = 0x361;

// Read right to left in documentation
typedef struct {
    uint8_t BootPermanentLocking : 1; // bit 0 Access to the vehicle (Boot permanent locking)
    uint8_t unknown1             : 1; // bit 1 
    uint8_t unknown2             : 1; // bit 2
    uint8_t unknown3             : 1; // bit 3
    uint8_t unknown4             : 1; // bit 4
    uint8_t unknown5             : 1; // bit 5
    uint8_t unknown6             : 1; // bit 6
    uint8_t unknown7             : 1; // bit 7
} CanDefineVehicleParametersByte1Struct;

typedef struct {
    uint8_t AutomaticLightingOfHeadlamps : 1; // bit 0 Lighting and signaling (Automatic ligthing of headlamps)
    uint8_t unknown1                     : 1; // bit 1 
    uint8_t unknown2                     : 1; // bit 2
    uint8_t unknown3                     : 1; // bit 3
    uint8_t GuideMeHomeHeadlamps         : 1; // bit 4 Lighting and signaling (Guide me home headlamps)
    uint8_t unknown5                     : 1; // bit 5
    uint8_t unknown6                     : 1; // bit 6
    uint8_t PlipActionDriverAllDoors     : 1; // bit 7 Access to the vehicle (Plip action Driver/All doors)
} CanDefineVehicleParametersByte2Struct;

typedef struct {
    uint8_t unknown0                   : 1; // bit 0
    uint8_t unknown1                   : 1; // bit 1 
    uint8_t unknown2                   : 1; // bit 2
    uint8_t unknown3                   : 1; // bit 3
    uint8_t unknown4                   : 1; // bit 4
    uint8_t ActivateRearWiperInReverse : 1; // bit 5 Parking assistance (Activation of rear wiper in reverse gear)
    uint8_t DayTimeRunningLamps        : 1; // bit 6 Lighting and signaling (Daytime-running lamps)
    uint8_t DirectionalHeadlamps       : 1; // bit 7 Driving assistance (Directional headlamps)
} CanDefineVehicleParametersByte3Struct;

typedef struct {
    uint8_t unknown0             : 1; // bit 0 
    uint8_t unknown1             : 1; // bit 1 
    uint8_t AmbianceLighting     : 1; // bit 2 Lighting and signaling (Ambiance lighting)
    uint8_t unknown3             : 1; // bit 3
    uint8_t unknown4             : 1; // bit 4
    uint8_t ParkingAssistance    : 1; // bit 5 Parking assistance (Parking assistance)
    uint8_t unknown6             : 1; // bit 6
    uint8_t unknown7             : 1; // bit 7
} CanDefineVehicleParametersByte4Struct;

// Read left to right in documentation
typedef struct CanDefineVehicleParametersStruct {
    uint8_t Field0;
    CanDefineVehicleParametersByte1Struct Parameters1;
    CanDefineVehicleParametersByte2Struct Parameters2;
    CanDefineVehicleParametersByte3Struct Parameters3;
    CanDefineVehicleParametersByte4Struct Parameters4;
    uint8_t Field5;
    uint8_t Field6;
    uint8_t Field7;
};

typedef union CanDefineVehicleParametersPacket {
    CanDefineVehicleParametersStruct data;
    uint8_t CanDefineVehicleParametersPacket[sizeof(CanDefineVehicleParametersStruct)];
};

#endif
