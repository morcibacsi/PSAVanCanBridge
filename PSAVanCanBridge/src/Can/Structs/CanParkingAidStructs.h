// CanParkingAidStructs.h
#pragma once

#ifndef _CanParkingAidStructs_h
    #define _CanParkingAidStructs_h

#include "../AbstractCanMessageSender.h"
#include "../../Helpers/PacketGenerator.h"

// CANID: 0E1
const uint16_t CAN_ID_PARKING_AID = 0x0E1;

enum PARK_POSITION
{
    PP_FRONT_LEFT  = 0,
    PP_FRONT       = 1,
    PP_FRONT_RIGHT = 2,
    PP_REAR_RIGHT  = 3,
    PP_REAR        = 4,
    PP_REAR_LEFT   = 5,
};

// Read right to left in documentation
typedef struct {
    uint8_t unknown             : 4; // bit 0-3
    uint8_t sound_enabled       : 1; // bit 4
    uint8_t channel             : 1; // bit 5     0: rear 1: front
    uint8_t left_channel_sound  : 1; // bit 6
    uint8_t right_channel_sound : 1; // bit 7
} CanParkingAidByte1Struct;

// Read right to left in documentation
typedef struct {
    uint8_t beep_period       : 6; // bit 0-5
    uint8_t unknown           : 2; // bit 6-7
} CanParkingAidByte2Struct;

// Read right to left in documentation
typedef struct {
    uint8_t unknown   : 2; // bit 0-1
    uint8_t rear      : 3; // bit 2-4
    uint8_t rear_left : 3; // bit 5-7
} CanParkingAidByte3Struct;

// Read right to left in documentation
typedef struct {
    uint8_t unknown    : 2; // bit 0-1
    uint8_t front_left : 3; // bit 2-4
    uint8_t rear_right : 3; // bit 5-7
} CanParkingAidByte4Struct;

// Read right to left in documentation
typedef struct {
    uint8_t unknown     : 1; // bit 0
    uint8_t show        : 1; // bit 1
    uint8_t front_right : 3; // bit 2-4
    uint8_t front       : 3; // bit 5-7
} CanParkingAidByte5Struct;

// Read left to right in documentation
struct CanParkingAidStruct {
    uint8_t Field1;
    CanParkingAidByte1Struct SoundOptions;
    CanParkingAidByte2Struct BeepPeriod;
    CanParkingAidByte3Struct RearAndRearLeft;
    CanParkingAidByte4Struct FronLeftAndRearRight;
    CanParkingAidByte5Struct FrontAndFrontRight;
    uint8_t Field7;
    uint8_t Field8;
};

union CanParkingAidPacket {
    CanParkingAidStruct data;
    uint8_t CanParkingAidPacket[sizeof(CanParkingAidStruct)];
};

#pragma region Sender class
class CanParkingAidPacketSender
{
    AbstractCanMessageSender* canMessageSender;

    uint8_t GetRearLeftBarCountFromCanByte(uint8_t value) {
        // 0   : 3
        // 1   : 2
        // 2   : 1
        // 3-7 : 0
        if (value == 0)
        {
            return 3;
        }
        if (value == 1)
        {
            return 2;
        }
        if (value == 2)
        {
            return 1;
        }
        if (value > 2)
        {
            return 0;
        }
    }

    uint8_t GetRearBarCountFromCanByte(uint8_t value) {
        // 0  : 4
        // 1-2: 3
        // 3-4: 2
        // 5-6: 1
        // 7  : 0
        if (value < 1)
        {
            return 4;
        }
        if (value <= 2)
        {
            return 3;
        }
        if (value <= 4)
        {
            return 2;
        }
        if (value <= 6)
        {
            return 1;
        }
        if (value == 7)
        {
            return 0;
        }
    }

    uint8_t GetFrontLeftBarCountFromCanByte(uint8_t value) {
        // 0  : 3
        // 1-2: 2
        // 3-4: 1
        // 5-7: 0
        if (value <= 1)
        {
            return 4;
        }
        if (value <= 2)
        {
            return 3;
        }
        if (value <= 4)
        {
            return 2;
        }
        if (value <= 6)
        {
            return 1;
        }
        if (value == 7)
        {
            return 0;
        }
    }

    uint8_t GetRearRightBarCountFromCanByte(uint8_t value) {
        // 0  : 3
        // 1  : 2
        // 2  : 1
        // 3-7: 0
        if (value == 0)
        {
            return 3;
        }
        if (value == 1)
        {
            return 2;
        }
        if (value == 2)
        {
            return 1;
        }
        if (value > 2)
        {
            return 0;
        }
    }

    uint8_t GetFrontBarCountFromCanByte(uint8_t value) {
        // 0  : 4
        // 1-2: 3
        // 3-4: 2
        // 5-6: 1
        // 7  : 0
        if (value < 1)
        {
            return 4;
        }
        if (value <= 2)
        {
            return 3;
        }
        if (value <= 4)
        {
            return 2;
        }
        if (value <= 6)
        {
            return 1;
        }
        if (value == 7)
        {
            return 0;
        }
    }

    uint8_t GetFrontRightBarCountFromCanByte(uint8_t value) {
        // 0  : 3
        // 1-2: 2
        // 3-4: 1
        // 5-7: 0
        if (value == 0)
        {
            return 3;
        }
        if (value <= 2)
        {
            return 2;
        }
        if (value <= 4)
        {
            return 1;
        }
        if (value <= 7)
        {
            return 0;
        }
    }

    uint8_t GetByteFromBarCount(uint8_t barCount, PARK_POSITION position) {
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
        return B111;
    }

    uint8_t GetBeepPeriod(uint8_t rearLeft, uint8_t rear, uint8_t rearRight, uint8_t frontLeft, uint8_t front, uint8_t frontRight)
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
            return 0x01;
        }
        if (max == 2)
        {
            return 0x04;
        }
        if (max == 1)
        {
            return 0x0C;
        }
        if (max == 0)
        {
            // silence
            return 0x3F;
        }
        return 0x3F;
    }

public:
    CanParkingAidPacketSender(AbstractCanMessageSender* object)
    {
        canMessageSender = object;
    }

    //rear and front maximum 4 bars
    //on the corners maxumum 3 bars
    void Send(uint8_t rearLeft, uint8_t rear, uint8_t rearRight, uint8_t frontLeft, uint8_t front, uint8_t frontRight, uint8_t soundEnabled)
    {
        PacketGenerator<CanParkingAidPacket> generator;

        generator.packet.data.SoundOptions.sound_enabled = soundEnabled;
        generator.packet.data.SoundOptions.channel = 1;
        generator.packet.data.SoundOptions.left_channel_sound = 1;
        generator.packet.data.SoundOptions.right_channel_sound = 1;

        generator.packet.data.BeepPeriod.beep_period = GetBeepPeriod(rearLeft, rear, rearRight, frontLeft, front, frontRight);

        generator.packet.data.RearAndRearLeft.rear = GetByteFromBarCount(rear, PP_REAR);
        generator.packet.data.RearAndRearLeft.rear_left = GetByteFromBarCount(rearLeft, PP_REAR_LEFT);

        generator.packet.data.FronLeftAndRearRight.front_left = GetByteFromBarCount(frontLeft, PP_FRONT_LEFT);
        generator.packet.data.FronLeftAndRearRight.rear_right = GetByteFromBarCount(rearRight, PP_REAR_RIGHT);

        //generator.packet.data.FrontAndFrontRight.front = GetByteFromBarCount(front, PP_FRONT);
        //generator.packet.data.FrontAndFrontRight.front_right = GetByteFromBarCount(front, PP_FRONT_RIGHT);

        generator.packet.data.FrontAndFrontRight.front = 7;
        generator.packet.data.FrontAndFrontRight.front_right = 7;
        generator.packet.data.FrontAndFrontRight.show = 1;

        unsigned char* serializedPacket = generator.GetSerializedPacket();
        canMessageSender->SendMessage(CAN_ID_PARKING_AID, 0, sizeof(CanParkingAidPacket), serializedPacket);
    }
};
#pragma endregion

#endif
