#pragma once

#ifndef _CanParkingAidHandler_h
    #define _CanParkingAidHandler_h

#include "../../../Config.h"
#include "../Handlers/CanMessageHandlerBase.h"
#include "../Structs/CanParkingAidStructs.h"
#include "../AbstractCanMessageSender.h"

class CanParkingAidHandler : public CanMessageHandlerBase
{
    static const int CAN_PARKING_AID_INTERVAL = 10;

    unsigned long _activeStarted = 0;
    unsigned long _lastTimeDataArrived = 0;

    uint8_t _isActive = 0;
    uint8_t _isTrailerPresent = 0;
    uint8_t _exteriorRearLeft = 0;
    uint8_t _exteriorRearRight = 0;
    uint8_t _interiorRearLeft = 0;
    uint8_t _interiorRearRight = 0;

    CanParkingAidPacketSender* packetSender;

    uint8_t GetBarCountFromDistance(uint8_t distanceData, bool isCorner)
    {
        uint8_t result = 0;

        uint8_t distanceBorders[] = { 80, 65, 50, 35, 35 }; //in centimeters
        uint8_t startingElement = 0;
        if (isCorner)
        {
            startingElement = 1;
        }

        if (distanceData < distanceBorders[startingElement])
        {
            result = 1;
        }
        if (distanceData < distanceBorders[startingElement+1])
        {
            result = 2;
        }
        if (distanceData < distanceBorders[startingElement+2])
        {
            result = 3;
        }
        if (distanceData < distanceBorders[startingElement+3])
        {
            result = 4;
        }

        return result;
    }

    void InternalProcess() override
    {
        if (_isActive == 1 && _isTrailerPresent == 0)
        {
            uint8_t rearLeft = GetBarCountFromDistance(_exteriorRearLeft, true);
            uint8_t rearRight = GetBarCountFromDistance(_exteriorRearRight, true);

            uint8_t intRearLeft = GetBarCountFromDistance(_interiorRearLeft, false);
            uint8_t intRearRight = GetBarCountFromDistance(_interiorRearRight, false);

            uint8_t rear = intRearLeft;
            if (intRearRight > rear)
            {
                rear = intRearRight;
            }

            packetSender->Send(rearLeft, rear, rearRight, 0, 0, 0, ENABLE_PARKING_AID_SOUND_FROM_SPEAKER);
        }
    }

    public:
    CanParkingAidHandler(AbstractCanMessageSender * object) : CanMessageHandlerBase(object, CAN_PARKING_AID_INTERVAL)
    {
        packetSender = new CanParkingAidPacketSender(object);
    }

    void SetData(uint8_t isActive, uint8_t isTrailerPresent, uint8_t exteriorRearLeft, uint8_t exteriorRearRight, uint8_t interiorRearLeft, uint8_t interiorRearRight, unsigned long currentTime)
    {
        _isTrailerPresent = isTrailerPresent;

        _exteriorRearLeft = exteriorRearLeft;
        _exteriorRearRight = exteriorRearRight;

        _interiorRearLeft = interiorRearLeft;
        _interiorRearRight = interiorRearRight;

        if (isActive)
        {
            _activeStarted = currentTime;
            _isActive = isActive;
        }
        else
        {
            if (currentTime - _activeStarted > 2000)
            {
                _isActive = isActive;
            }
        }
    }
};

#endif
