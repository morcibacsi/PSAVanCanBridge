// VanParkingAidDiagStructs.h
#pragma once

#ifndef _VanParkingAidDiagStructs_h
    #define _VanParkingAidDiagStructs_h

#include <stdint.h>

// VANID: AE8
const uint16_t VAN_ID_PARKING_AID_DIAG_ANSWER = 0xAE8;
// VANID: A68
const uint16_t VAN_ID_PARKING_AID_DIAG_QUERY = 0xA68;

const uint8_t PR_DIAG_ANSWER_IDENTIFICATION = 0x80;
const uint8_t PR_DIAG_ANSWER_STATE_OF_INPUT = 0xC5;
const uint8_t PR_DIAG_ANSWER_DISTANCE       = 0xA0;
const uint8_t PR_DIAG_ANSWER_CONFIG         = 0xC0;

#pragma region Distance
// Read left to right in documentation
struct VanParkingAidDiagDistanceStruct {
    uint8_t Header;
    uint8_t Byte1;
    uint8_t DiagFunctionId;
    uint8_t ExteriorRearLeftDistanceInCm;
    uint8_t ExteriorRearRightDistanceInCm;
    uint8_t InteriorRearLeftDistanceInCm;
    uint8_t InteriorRearRightDistanceInCm;
    uint8_t Byte7;
    uint8_t Byte8;
    uint8_t Byte9;
    uint8_t Byte10;
    uint8_t Byte11;
    uint8_t Byte12;
    uint8_t Byte13;
    uint8_t Byte14;
    uint8_t Byte15;
    uint8_t Byte16;
    uint8_t Byte17;
    uint8_t Byte18;
    uint8_t Byte19;
    uint8_t Byte20;
    uint8_t Byte21;
    uint8_t Byte22;
    uint8_t Footer;
};

union VanParkingAidDiagDistancePacket {
    VanParkingAidDiagDistanceStruct data;
    uint8_t VanParkingAidDiagDistancePacket[sizeof(VanParkingAidDiagDistanceStruct)];
};
#pragma endregion

#pragma region Configuration

enum sensors_installed
{
    //2 sensors
    sensors_2        = 1,
    //4 sensors
    sensors_4        = 2,
    //2 rear + 2 front
    sensors_2_plus_2 = 3,
    //4 rear + 4 front
    sensors_4_plus_4 = 4,
};

enum control_of_the_speakers
{
    local = 0,
    van_network = 1
};

typedef struct {
    uint8_t sensors_installed            : 3; // bit 0-2
    uint8_t presence_of_disarming_switch : 1; // bit 3
    uint8_t unknown_4                    : 1; // bit 4
    uint8_t unknown_5                    : 1; // bit 5
    uint8_t unknown_6                    : 1; // bit 6
    uint8_t unknown_7                    : 1; // bit 7
} VanParkingAidConfigByte3Struct;

typedef struct {
    uint8_t unknown_0                    : 1; // bit 0
    uint8_t control_of_the_speakers      : 1; // bit 1
    uint8_t unknown_2                    : 1; // bit 2
    uint8_t unknown_3                    : 1; // bit 3
    uint8_t unknown_4                    : 1; // bit 4
    uint8_t unknown_5                    : 1; // bit 5
    uint8_t unknown_6                    : 1; // bit 6
    uint8_t unknown_7                    : 1; // bit 7
} VanParkingAidConfigByte4Struct;


// Read left to right in documentation
struct VanParkingAidDiagConfigurationStruct {
    uint8_t Header;
    uint8_t Byte1;
    uint8_t DiagFunctionId;
    VanParkingAidConfigByte3Struct SensorsAndDisarm;
    VanParkingAidConfigByte4Struct Speakers;
    uint8_t Byte5;
    uint8_t Byte6;
    uint8_t Byte7;
    uint8_t Byte8;
    uint8_t Byte9;
    uint8_t Byte10;
    uint8_t RearInteriorSensorMaxDetectionDistanceInCm;
    uint8_t RearExteriorSensorMaxDetectionDistanceInCm;
    uint8_t Byte13;
    uint8_t Byte14;
    uint8_t Byte15;
    uint8_t Byte16;
    uint8_t Byte17;
    uint8_t Byte18;
    uint8_t Byte19;
    uint8_t Byte20;
    uint8_t Byte21;
    uint8_t Footer;
};

union VanParkingAidDiagConfigurationPacket {
    VanParkingAidDiagConfigurationStruct data;
    uint8_t VanParkingAidDiagConfigurationPacket[sizeof(VanParkingAidDiagConfigurationStruct)];
};
#pragma endregion

#pragma region Fault codes

// Read left to right in documentation
struct VanParkingAidDiagFaultCodesStruct {
    uint8_t Header;
    uint8_t Byte1;
    uint8_t DiagFunctionId;
    uint8_t Byte3;
    uint8_t Byte4;
    uint8_t Byte5;
    uint8_t Byte6;
    uint8_t Byte7;
    uint8_t Byte8;
    uint8_t Byte9;
    uint8_t Byte10;
    uint8_t Byte11;
    uint8_t Byte12;
    uint8_t Byte13;
    uint8_t Byte14;
    uint8_t Byte15;
    uint8_t Byte16;
    uint8_t Byte17;
    uint8_t Byte18;
    uint8_t Byte19;
    uint8_t Byte20;
    uint8_t Byte21;
    uint8_t Byte22;
    uint8_t Byte23;
    uint8_t Byte24;
    uint8_t Byte25;
    uint8_t Byte26;
    uint8_t Footer;
};

union VanParkingAidDiagFaultCodesPacket {
    VanParkingAidDiagFaultCodesStruct data;
    uint8_t VanParkingAidDiagFaultCodesPacket[sizeof(VanParkingAidDiagFaultCodesStruct)];
};
#pragma endregion

#pragma region Inputs

typedef struct {
    uint8_t system_active   : 1; // bit 0
    uint8_t unknown_1       : 1; // bit 1
    uint8_t unknown_2       : 1; // bit 2
    uint8_t trailer_present : 1; // bit 3
    uint8_t unknown_4       : 1; // bit 4
    uint8_t unknown_5       : 1; // bit 5
    uint8_t unknown_6       : 1; // bit 6
    uint8_t unknown_7       : 1; // bit 7
} VanParkingAidInputStateByte3Struct;

// Read left to right in documentation
struct VanParkingAidInputStateStruct {
    uint8_t Header;
    uint8_t Byte1;
    uint8_t DiagFunctionId;
    VanParkingAidInputStateByte3Struct Status;
    uint8_t Footer;
};

union VanParkingAidInputStatePacket {
    VanParkingAidInputStateStruct data;
    uint8_t VanParkingAidInputStatePacket[sizeof(VanParkingAidInputStateStruct)];
};
#pragma endregion

#endif
