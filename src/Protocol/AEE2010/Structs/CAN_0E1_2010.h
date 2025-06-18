#pragma once

#ifndef _CAN_0E1_2010_h
    #define _CAN_0E1_2010_h

#include <stdint.h>

// CANID: 0E1
const uint16_t CAN_ID_PARKING_AID_2010 = 0x0E1;

enum PARK_POSITION
{
    PP_FRONT_LEFT  = 0,
    PP_FRONT       = 1,
    PP_FRONT_RIGHT = 2,
    PP_REAR_RIGHT  = 3,
    PP_REAR        = 4,
    PP_REAR_LEFT   = 5,
};

const uint8_t AAS_STATUS_NOT_DEFINED_2010        = 0b000;
const uint8_t AAS_STATUS_FAULT_2010              = 0b001;
const uint8_t AAS_STATUS_DISABLED_BY_DRIVER_2010 = 0b010;
const uint8_t AAS_STATUS_ACTIVE_2010             = 0b100;
const uint8_t AAS_STATUS_WAIT_2010               = 0b101;
const uint8_t AAS_STATUS_OUT_OF_SERVICE_2010     = 0b110;
const uint8_t AAS_STATUS_NOT_DEFINED2_2010       = 0b111;

const uint8_t AAS_DISTANCE_ZONE1_NEAR_2010 = 0b000;
const uint8_t AAS_DISTANCE_ZONE2_2010      = 0b001;
const uint8_t AAS_DISTANCE_ZONE3_2010      = 0b010;
const uint8_t AAS_DISTANCE_ZONE4_2010      = 0b011;
const uint8_t AAS_DISTANCE_ZONE5_2010      = 0b100;
const uint8_t AAS_DISTANCE_ZONE6_2010      = 0b101;
const uint8_t AAS_DISTANCE_ZONE7_2010      = 0b110;
const uint8_t AAS_DISTANCE_ZONE8_FAR_2010  = 0b111;

const uint8_t AAS_BEEP_DIRECTION_NO_BEEP_2010        = 0b00;
const uint8_t AAS_BEEP_DIRECTION_LEFT_2010           = 0b01;
const uint8_t AAS_BEEP_DIRECTION_RIGHT_2010          = 0b10;
const uint8_t AAS_BEEP_DIRECTION_LEFT_AND_RIGHT_2010 = 0b11;

const uint8_t AAS_BEEP_DURATION_1_2010 = 0b00;
const uint8_t AAS_BEEP_DURATION_2_2010 = 0b01;
const uint8_t AAS_BEEP_DURATION_3_2010 = 0b10;
const uint8_t AAS_BEEP_DURATION_4_2010 = 0b11;

const uint8_t AAS_SOUND_LOCATION_REAR_2010  = 0;
const uint8_t AAS_SOUND_LOCATION_FRONT_2010 = 1;

union CanParkingAidByte1_2010Struct {
    struct
    {
        uint8_t measurement_side : 2; // bit 0-1
        uint8_t front_status     : 3; // bit 2-4
        uint8_t rear_status      : 3; // bit 5-7
    } data;
    uint8_t asByte;
};

// Read right to left in documentation
union CanParkingAidByte2_2010Struct
{
    struct {
        uint8_t               : 4; // bit 0-3
        uint8_t sound_enabled : 1; // bit 4
        uint8_t location      : 1; // bit 5     0: rear 1: front
        uint8_t channel       : 2; // bit 6-7   0: no beep, 1: left, 2: right, 3: both
    } data;
    uint8_t asByte;
};

// Read right to left in documentation
union CanParkingAidByte3_2010Struct
{
    struct {
        uint8_t beep_delay        : 6; // bit 0-5   in ms
        uint8_t beep_duration     : 2; // bit 6-7
    } data;
    uint8_t asByte;
};

// Read right to left in documentation
union CanParkingAidByte4_2010Struct {
    struct {
        uint8_t                    : 2; // bit 0-1
        uint8_t rear_distance      : 3; // bit 2-4
        uint8_t rear_left_distance : 3; // bit 5-7
    } data;
    uint8_t asByte;
};

// Read right to left in documentation
union CanParkingAidByte5_2010Struct {
    struct {
        uint8_t                     : 2; // bit 0-1
        uint8_t front_left_distance : 3; // bit 2-4
        uint8_t rear_right_distance : 3; // bit 5-7
    } data;
    uint8_t asByte;
};

// Read right to left in documentation
union CanParkingAidByte6_2010Struct {
    struct {
        uint8_t                      : 1; // bit 0
        uint8_t show                 : 1; // bit 1
        uint8_t front_right_distance : 3; // bit 2-4
        uint8_t front_distance       : 3; // bit 5-7
    } data;
    uint8_t asByte;
};

union CanParkingAidByte7_2010Struct {
    struct {
        uint8_t                                 : 1; // bit 0
        uint8_t which_side_was_measured         : 1; // bit 1      0: left, 1: right
        uint8_t show_measured_space_on_display  : 1; // bit 2
        uint8_t measured_free_space             : 2; // bit 3-4    0:no space, 1: small space, 2: medium space, 3: large space
        uint8_t measurement_status              : 3; // bit 5-7    2: off, 4: active
    } data;
    uint8_t asByte;
};
// Read left to right in documentation

struct CanParkingAid_2010Struct {
    CanParkingAidByte1_2010Struct Status;
    CanParkingAidByte2_2010Struct SoundOptions;
    CanParkingAidByte3_2010Struct BeepPeriod;
    CanParkingAidByte4_2010Struct RearAndRearLeft;
    CanParkingAidByte5_2010Struct FronLeftAndRearRight;
    CanParkingAidByte6_2010Struct FrontAndFrontRight;
    CanParkingAidByte7_2010Struct Measurement;
};

union CanParkingAid_2010Packet {
    CanParkingAid_2010Struct data;
    uint8_t CanParkingAid_2010Packet[sizeof(CanParkingAid_2010Struct)];
};

uint8_t GetByteFromBarCount_2010(uint8_t barCount, PARK_POSITION position) {
    //bar values are at the index of the array

    if (position == PP_FRONT || position == PP_REAR)
    {
        if (barCount > 4)
        {
            barCount = 4;
        }
    }
    if (position == PP_FRONT_LEFT || position == PP_FRONT_RIGHT || position == PP_REAR_LEFT || position == PP_REAR_RIGHT)
    {
        if (barCount > 3)
        {
            barCount = 3;
        }
    }

    if (position == PP_REAR_RIGHT || position == PP_REAR_LEFT)
    {
        uint8_t dataRR_RL[] = { 3, 2, 1, 0 };
        return dataRR_RL[barCount];
    }
    if (position == PP_REAR || position == PP_FRONT)
    {
        uint8_t dataR_F[]  = { 7, 5, 3, 1, 0 };
        return dataR_F[barCount];
    }

    if (position == PP_FRONT_RIGHT || position == PP_FRONT_LEFT)
    {
        uint8_t dataFR_FL[] = { 7, 3, 1, 0 };
        return dataFR_FL[barCount];
    }
    return 7;//B111;
}

uint8_t GetBeepPeriod_2010(uint8_t rearLeft, uint8_t rear, uint8_t rearRight, uint8_t frontLeft, uint8_t front, uint8_t frontRight)
{
    uint8_t max = rear;

    if (rearLeft > max)
    {
        max = rearLeft;
    }
    if (rearRight > max)
    {
        max = rearRight;
    }

    if (rearRight == 3 || rearLeft == 3)
    {
        max = 4;
    }

    if (max == 4)
    {
        // continuous beeping
        return 0x00;
    }
    if (max == 3)
    {
        return 0x01;//1
    }
    if (max == 2)
    {
        return 0x04;//4
    }
    if (max == 1)
    {
        return 0x0C;//12
    }
    if (max == 0)
    {
        // silence
        return 0x3F;//63
    }
    return 0x3F;
}
#endif
