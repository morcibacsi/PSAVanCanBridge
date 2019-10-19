// CanDisplayStatusStructs.h
#pragma once

#ifndef _CanDisplayStatusStructs_h
    #define _CanDisplayStatusStructs_h

// CANID: 167
const uint16_t CAN_ID_DISPLAYSTATUS = 0x167;

// Read right to left in documentation
typedef struct {
    uint8_t unknown0 : 1; // bit 0
    uint8_t unknown1 : 1; // bit 1 
    uint8_t unknown2 : 1; // bit 2
    uint8_t unknown3 : 1; // bit 3
    uint8_t unknown4 : 1; // bit 4
    uint8_t DarkMode : 1; // bit 5
    uint8_t unknown6 : 1; // bit 6
    uint8_t unknown7 : 1; // bit 7
} CanDisplayStatusByte0Struct;

typedef struct {
    uint16_t Km : 16;
} CanDisplayStatusTripDestinationDistanceStruct;


// Read left to right in documentation
typedef struct CanDisplayStatusStruct {
    uint8_t Field1;
    CanDisplayStatusByte0Struct Field2;
    CanDisplayStatusTripDestinationDistanceStruct DistanceToDestination;
    uint8_t Field5;
    uint8_t Field6;
    uint8_t Field7;
    uint8_t Field8;
};

typedef union CanDisplayStatusPacket {
    CanDisplayStatusStruct data;
    uint8_t CanDisplayStatusPacket[sizeof(CanDisplayStatusStruct)];
};

uint16_t GetKmFromCanData(uint16_t canData)
{
    return canData / 2;
}
#endif
