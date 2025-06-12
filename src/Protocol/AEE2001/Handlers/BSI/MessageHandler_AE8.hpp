#pragma once

#ifndef _MessageHandler_AE8_h
    #define _MessageHandler_AE8_h

#include <cstdint>
#include <cstring>

#include "../../Structs/VAN_8A4.h"
#include "../../../IMessageHandler.hpp"

class MessageHandler_AE8 : public IMessageHandler<MessageHandler_AE8>
{
    uint8_t GetMinDistance(uint8_t rearLeftExt, uint8_t rearRightExt, uint8_t rearLeftInt, uint8_t rearRightInt, uint8_t *isCorner)
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

    uint8_t GetBeepDirection(uint8_t rearLeftExt, uint8_t rearRightExt, uint8_t rearLeftInt, uint8_t rearRightInt)
    {
        uint8_t result = static_cast<uint8_t>(ParkingAidBeepDirection::LeftAndRight);

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
            result = static_cast<uint8_t>(ParkingAidBeepDirection::Left);
        }

        if (right < center)
        {
            result = static_cast<uint8_t>(ParkingAidBeepDirection::Right);
        }

        return result;
    }

    uint8_t GetBarCountFromDistance(CarState* carState, uint8_t distanceData, bool isCorner)
    {
        uint8_t result = static_cast<uint8_t>(ParkingAidBarCount::Zone8Far);

        if (isCorner)
        {
            if (distanceData <= carState->AAS_REAR_CORNER_DISTANCES[0])
            {
                result = static_cast<uint8_t>(ParkingAidBarCount::Zone4);
            }
            if (distanceData <= carState->AAS_REAR_CORNER_DISTANCES[1])
            {
                result = static_cast<uint8_t>(ParkingAidBarCount::Zone3);
            }
            if (distanceData <= carState->AAS_REAR_CORNER_DISTANCES[2])
            {
                result = static_cast<uint8_t>(ParkingAidBarCount::Zone2);
            }
            if (distanceData <= carState->AAS_REAR_CORNER_DISTANCES[3])
            {
                result = static_cast<uint8_t>(ParkingAidBarCount::Zone1Near);
            }
        }
        else
        {
            if (distanceData <= carState->AAS_REAR_DISTANCES[0])
            {
                result = static_cast<uint8_t>(ParkingAidBarCount::Zone7);
            }
            if (distanceData <= carState->AAS_REAR_DISTANCES[1])
            {
                result = static_cast<uint8_t>(ParkingAidBarCount::Zone5);
            }
            if (distanceData <= carState->AAS_REAR_DISTANCES[2])
            {
                result = static_cast<uint8_t>(ParkingAidBarCount::Zone3);
            }
            if (distanceData <= carState->AAS_REAR_DISTANCES[3])
            {
                result = static_cast<uint8_t>(ParkingAidBarCount::Zone1Near);
            }
        }

        return result;
    }

    uint8_t GetBeepDelayFromDistance(CarState* carState, uint8_t minDistance, bool isCorner)
    {
        //0-62
        uint8_t result = 62;

        if (isCorner == 1)
        {
            if (minDistance < carState->AAS_REAR_CORNER_DISTANCES[0])
            {
                result = 62;
            }
            if (minDistance <= carState->AAS_REAR_CORNER_DISTANCES[1])
            {
                result = 40;
            }
            if (minDistance <= carState->AAS_REAR_CORNER_DISTANCES[2])
            {
                result = 20;
            }
            if (minDistance <= carState->AAS_REAR_CORNER_DISTANCES[3])
            {
                result = 0;
            }
        }
        else
        {
            if (minDistance <= carState->AAS_REAR_DISTANCES[0])
            {
                //the delay is decremented by 1 for every 2 cm when the distance is between 100-145 (starting from 62)

                //23 is the number of delay steps between the "1 bar distance" and the "2 bar distance"
                //(145 cm - 100cm )/23 = ~2
                //62 is the default delay
                result = 62 - ((carState->AAS_REAR_DISTANCES[0]-minDistance)/2);
            }
            if (minDistance <= carState->AAS_REAR_DISTANCES[1])
            {
                //the delay is decremented by 1 for every 2.5 cm when the distance is between 50-100 (starting from 40)

                //20 is the number of delay steps between the "2 bar distance" and the "3 bar distance"
                //(100 cm - 50cm )/20 = ~2.5
                //40 is the delay of the first 2 bar distance
                result = 40 - ((carState->AAS_REAR_DISTANCES[1]-minDistance)/2.5);
            }
            if (minDistance <= carState->AAS_REAR_DISTANCES[2])
            {
                //the delay is decremented by 1 for every 2.5 cm when the distance is between 30-50 (starting from 20)

                //the last delay before we reach "4 bar distance" is 12, so 8 = 20-12
                //(50 cm - 30cm )/8 = ~2.5
                //20 is the delay of the first 3 bar distance
                result = 20 - ((carState->AAS_REAR_DISTANCES[2]-minDistance)/2.5);
            }
            if (minDistance <= carState->AAS_REAR_DISTANCES[3])
            {
                //if the distance reaches 30 cm the delay is 0
                result = 0;
            }
        }

        return result;
    }

    uint8_t GetBeepDurationFromDistance(CarState* carState, uint8_t minDistance, bool isCorner)
    {
        //in a 5008 it is always 0
        return 0;

        uint8_t result = static_cast<uint8_t>(ParkingAidBeepDuration::Duration4);

        if (isCorner == 1)
        {
            if (minDistance < carState->AAS_REAR_CORNER_DISTANCES[3])
            {
                result = static_cast<uint8_t>(ParkingAidBeepDuration::Duration3);
            }
            if (minDistance < carState->AAS_REAR_CORNER_DISTANCES[2])
            {
                result = static_cast<uint8_t>(ParkingAidBeepDuration::Duration2);
            }
            if (minDistance < carState->AAS_REAR_CORNER_DISTANCES[0])
            {
                result = static_cast<uint8_t>(ParkingAidBeepDuration::Duration1);
            }
        }
        else
        {
            if (minDistance < carState->AAS_REAR_DISTANCES[3])
            {
                result = static_cast<uint8_t>(ParkingAidBeepDuration::Duration3);
            }
            if (minDistance < carState->AAS_REAR_DISTANCES[2])
            {
                result = static_cast<uint8_t>(ParkingAidBeepDuration::Duration2);
            }
            if (minDistance < carState->AAS_REAR_DISTANCES[0])
            {
                result = static_cast<uint8_t>(ParkingAidBeepDuration::Duration1);
            }
        }

        return result;
    }

    public:
        static constexpr uint32_t MessageId = 0xAE8;

        BusMessage Generate(CarState* carState)
        {
            BusMessage message;
            message.id = MessageId;
            message.periodicityMs = 500;
            message.offsetMs = 20;
            message.protocol = ProtocolType::AEE2001;
            message.type = MessageType::Query;
            message.isActive = false; //it is sent as a feedback message when A68 is received

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {
            if (message.dataLength != 24 || carState->PARKING_AID_TYPE != 1)
            {
                return;
            }

            if (message.data[2] == 0xA0)
            {
                uint8_t ParkingExteriorRearLeft  = message.data[3];
                uint8_t ParkingExteriorRearRight = message.data[4];
                uint8_t ParkingInteriorRearLeft  = message.data[5];
                uint8_t ParkingInteriorRearRight = message.data[6];

                uint8_t isCorner = 0;
                uint8_t minDistance = GetMinDistance(
                            ParkingExteriorRearLeft,
                            ParkingExteriorRearRight,
                            ParkingInteriorRearLeft,
                            ParkingInteriorRearRight,
                            &isCorner);

                carState->ParkingAidStatus.data.RearStatus =
                carState->IsReverseEngaged == 1
                    ? static_cast<uint8_t>(ParkingAidStatus::Active)
                    : static_cast<uint8_t>(ParkingAidStatus::Disabled);

                uint8_t enableSoundFromSpeaker = 0;
                if (carState->ENABLE_PARKING_AID_SOUND_FROM_SPEAKER)
                {
                    enableSoundFromSpeaker = 1;
                }

                carState->ParkingAidStatus.data.SoundEnabled = carState->IsReverseEngaged && enableSoundFromSpeaker && minDistance < 0xFF;
                carState->ParkingAidStatus.data.Location = static_cast<uint8_t>(ParkingAidLocation::Rear); //there are no front sensors on VAN, only rear is supported
                carState->ParkingAidStatus.data.Channel = GetBeepDirection(
                            ParkingExteriorRearLeft,
                            ParkingExteriorRearRight,
                            ParkingInteriorRearLeft,
                            ParkingInteriorRearRight);

                uint8_t rearLeft  = GetBarCountFromDistance(carState, ParkingExteriorRearLeft, true);
                uint8_t rearRight = GetBarCountFromDistance(carState, ParkingExteriorRearRight, true);

                uint8_t intRearLeft  = GetBarCountFromDistance(carState, ParkingInteriorRearLeft, false);
                uint8_t intRearRight = GetBarCountFromDistance(carState, ParkingInteriorRearRight, false);

                uint8_t rear = intRearLeft;

                if (intRearRight < rear)
                {
                    rear = intRearRight;
                }

                carState->ParkingAidStatus.data.BeepDelay = GetBeepDelayFromDistance(carState, minDistance, isCorner);
                carState->ParkingAidStatus.data.BeepDuration = GetBeepDurationFromDistance(carState, minDistance, isCorner);

                carState->ParkingAidStatus.data.RearDistance = rear;
                carState->ParkingAidStatus.data.RearLeftDistance = rearLeft;
                carState->ParkingAidStatus.data.RearRightDistance = rearRight;
                carState->ParkingAidStatus.data.FrontDistance = static_cast<uint8_t>(ParkingAidBarCount::Zone8Far);
                carState->ParkingAidStatus.data.FrontLeftDistance = static_cast<uint8_t>(ParkingAidBarCount::Zone8Far);
                carState->ParkingAidStatus.data.FrontRightDistance = static_cast<uint8_t>(ParkingAidBarCount::Zone8Far);
            }
        }
};
#endif
