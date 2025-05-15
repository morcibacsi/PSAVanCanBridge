#include <inttypes.h>
#include <memory>
#include <cstring>
#include "../../../IMessageHandler.hpp"
#include "../../Structs/CAN_0E1.h"
#include "MessageHandler_0E1.hpp"

BusMessage MessageHandler_0E1::Generate(CarState* state)

{
    uint8_t isCorner = 0;
    uint8_t minDistance = GetMinDistance(
                            state->ParkingExteriorRearLeft,
                            state->ParkingExteriorRearRight,
                            state->ParkingInteriorRearLeft,
                            state->ParkingInteriorRearRight,
                            &isCorner);

    CanParkingAidByte1Struct status;
    status.data.rear_status = state->IsReverseEngaged == 1 ? AAS_STATUS_ACTIVE : 0;

    CanParkingAidByte2Struct soundOptions;
    soundOptions.data.sound_enabled = state->ParkingSoundEnabled && state->IsReverseEngaged && state->HasParkingRadarData && minDistance < 0xFF;
    soundOptions.data.location      = AAS_SOUND_LOCATION_REAR; //there are no front sensors on VAN, only rear is supported
    soundOptions.data.channel       = GetBeepDirection(
                                        state->ParkingExteriorRearLeft,
                                        state->ParkingExteriorRearRight,
                                        state->ParkingInteriorRearLeft,
                                        state->ParkingInteriorRearRight);

    uint8_t rearLeft  = GetBarCountFromDistance(state, state->ParkingExteriorRearLeft, true);
    uint8_t rearRight = GetBarCountFromDistance(state, state->ParkingExteriorRearRight, true);

    uint8_t intRearLeft  = GetBarCountFromDistance(state, state->ParkingInteriorRearLeft, false);
    uint8_t intRearRight = GetBarCountFromDistance(state, state->ParkingInteriorRearRight, false);

    uint8_t rear = intRearLeft;

    if (intRearRight < rear)
    {
        rear = intRearRight;
    }

    CanParkingAidByte3Struct beep{};
    beep.data.beep_delay    = GetBeepDelayFromDistance(state, minDistance, isCorner);
    beep.data.beep_duration = GetBeepDurationFromDistance(state, minDistance, isCorner);

    CanParkingAidByte4Struct rearAndRearLeft{};
    rearAndRearLeft.data.rear_distance      = rear;
    rearAndRearLeft.data.rear_left_distance = rearLeft;

    CanParkingAidByte5Struct frontLeftAndRearRight{};
    frontLeftAndRearRight.data.rear_right_distance = rearRight;
    frontLeftAndRearRight.data.front_left_distance = AAS_DISTANCE_ZONE8_FAR;

    CanParkingAidByte6Struct field6{};
    field6.data.show = state->IsReverseEngaged && state->HasParkingRadarData;
    field6.data.front_right_distance = AAS_DISTANCE_ZONE8_FAR;
    field6.data.front_distance       = AAS_DISTANCE_ZONE8_FAR;

    message.data[0] = status.asByte;
    message.data[1] = soundOptions.asByte;
    message.data[2] = beep.asByte;
    message.data[3] = rearAndRearLeft.asByte;
    message.data[4] = frontLeftAndRearRight.asByte;
    message.data[5] = field6.asByte;
    message.data[6] = 0x00;

    return message;
    //*/
}

void MessageHandler_0E1::Parse(CarState* carState, const BusMessage& message)
{

}

uint8_t MessageHandler_0E1::GetBarCountFromDistance(CarState* state, uint8_t distanceData, bool isCorner)
{
    uint8_t result = AAS_DISTANCE_ZONE8_FAR;

    if (isCorner)
    {
        if (distanceData <= state->AAS_REAR_CORNER_DISTANCES[0])
        {
            result = AAS_DISTANCE_ZONE4;
        }
        if (distanceData <= state->AAS_REAR_CORNER_DISTANCES[1])
        {
            result = AAS_DISTANCE_ZONE3;
        }
        if (distanceData <= state->AAS_REAR_CORNER_DISTANCES[2])
        {
            result = AAS_DISTANCE_ZONE2;
        }
        if (distanceData <= state->AAS_REAR_CORNER_DISTANCES[3])
        {
            result = AAS_DISTANCE_ZONE1_NEAR;
        }
    }
    else
    {
        if (distanceData <= state->AAS_REAR_DISTANCES[0])
        {
            result = AAS_DISTANCE_ZONE7;
        }
        if (distanceData <= state->AAS_REAR_DISTANCES[1])
        {
            result = AAS_DISTANCE_ZONE5;
        }
        if (distanceData <= state->AAS_REAR_DISTANCES[2])
        {
            result = AAS_DISTANCE_ZONE3;
        }
        if (distanceData <= state->AAS_REAR_DISTANCES[3])
        {
            result = AAS_DISTANCE_ZONE1_NEAR;
        }
    }

    return result;
}

uint8_t MessageHandler_0E1::GetBeepDelayFromDistance(CarState* state, uint8_t minDistance, bool isCorner)
{
    //0-62
    uint8_t result = 62;

    if (isCorner == 1)
    {
        if (minDistance < state->AAS_REAR_CORNER_DISTANCES[0])
        {
            result = 62;
        }
        if (minDistance <= state->AAS_REAR_CORNER_DISTANCES[1])
        {
            result = 40;
        }
        if (minDistance <= state->AAS_REAR_CORNER_DISTANCES[2])
        {
            result = 20;
        }
        if (minDistance <= state->AAS_REAR_CORNER_DISTANCES[3])
        {
            result = 0;
        }
    }
    else
    {
        if (minDistance <= state->AAS_REAR_DISTANCES[0])
        {
            //the delay is decremented by 1 for every 2 cm when the distance is between 100-145 (starting from 62)

            //23 is the number of delay steps between the "1 bar distance" and the "2 bar distance"
            //(145 cm - 100cm )/23 = ~2
            //62 is the default delay
            result = 62 - ((state->AAS_REAR_DISTANCES[0]-minDistance)/2);
        }
        if (minDistance <= state->AAS_REAR_DISTANCES[1])
        {
            //the delay is decremented by 1 for every 2.5 cm when the distance is between 50-100 (starting from 40)

            //20 is the number of delay steps between the "2 bar distance" and the "3 bar distance"
            //(100 cm - 50cm )/20 = ~2.5
            //40 is the delay of the first 2 bar distance
            result = 40 - ((state->AAS_REAR_DISTANCES[1]-minDistance)/2.5);
        }
        if (minDistance <= state->AAS_REAR_DISTANCES[2])
        {
            //the delay is decremented by 1 for every 2.5 cm when the distance is between 30-50 (starting from 20)

            //the last delay before we reach "4 bar distance" is 12, so 8 = 20-12
            //(50 cm - 30cm )/8 = ~2.5
            //20 is the delay of the first 3 bar distance
            result = 20 - ((state->AAS_REAR_DISTANCES[2]-minDistance)/2.5);
        }
        if (minDistance <= state->AAS_REAR_DISTANCES[3])
        {
            //if the distance reaches 30 cm the delay is 0
            result = 0;
        }
    }

    return result;
}

uint8_t MessageHandler_0E1::GetBeepDurationFromDistance(CarState* state, uint8_t minDistance, bool isCorner)
{
    //in a 5008 it is always 0
    return 0;

    uint8_t result = AAS_BEEP_DURATION_4;

    if (isCorner == 1)
    {
        if (minDistance < state->AAS_REAR_CORNER_DISTANCES[3])
        {
            result = AAS_BEEP_DURATION_3;
        }
        if (minDistance < state->AAS_REAR_CORNER_DISTANCES[2])
        {
            result = AAS_BEEP_DURATION_2;
        }
        if (minDistance < state->AAS_REAR_CORNER_DISTANCES[0])
        {
            result = AAS_BEEP_DURATION_1;
        }
    }
    else
    {
        if (minDistance < state->AAS_REAR_DISTANCES[3])
        {
            result = AAS_BEEP_DURATION_3;
        }
        if (minDistance < state->AAS_REAR_DISTANCES[2])
        {
            result = AAS_BEEP_DURATION_2;
        }
        if (minDistance < state->AAS_REAR_DISTANCES[0])
        {
            result = AAS_BEEP_DURATION_1;
        }
    }

    return result;
}

uint8_t MessageHandler_0E1::GetBeepDirection(uint8_t rearLeftExt, uint8_t rearRightExt, uint8_t rearLeftInt, uint8_t rearRightInt)
{
    uint8_t result = AAS_BEEP_DIRECTION_LEFT_AND_RIGHT;

    uint8_t center = rearLeftInt;
    if (rearRightInt < rearLeftInt)
    {
        center = rearRightInt;
    }

    uint8_t left = rearLeftInt;
    if (rearLeftExt < rearLeftInt)
    {
        left = rearLeftExt;
    }

    uint8_t right = rearRightInt;
    if (rearRightExt < rearRightInt)
    {
        right = rearRightExt;
    }

    if (left < center)
    {
        result = AAS_BEEP_DIRECTION_LEFT;
    }

    if (right < center)
    {
        result = AAS_BEEP_DIRECTION_RIGHT;
    }

    return result;
}

uint8_t MessageHandler_0E1::GetMinDistance(uint8_t rearLeftExt, uint8_t rearRightExt, uint8_t rearLeftInt, uint8_t rearRightInt, uint8_t *isCorner)
{
    uint8_t values[4];
    values[0] = rearLeftExt;
    values[1] = rearRightExt;
    values[2] = rearLeftInt;
    values[3] = rearRightInt;

    uint8_t result = values[0];
    *isCorner = 1;

    for (uint8_t i = 1; i < 4; i++)
    {
        if (values[i] < result)
        {
            if (i > 1)
            {
                *isCorner = 0;
            }

            result = values[i];
        }
    }

    return result;
}
