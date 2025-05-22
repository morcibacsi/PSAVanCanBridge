#pragma once

#ifndef _MessageHandler_0E1_h
    #define _MessageHandler_0E1_h

#include <cstdint>
#include <cstring>

#include "../../../IMessageHandler.hpp"

class MessageHandler_0E1 : public IMessageHandler<MessageHandler_0E1>
{
    private:
        BusMessage message
        {
            .id = 0x0E1,
            .data = {0},
            .dataLength = 7,
            .protocol = ProtocolType::AEE2004,
            .periodicityMs = 100,
            .offsetMs = 0,
            .isActive = true
        };

        uint8_t GetBarCountFromDistance(CarState* carState, uint8_t distanceData, bool isCorner);
        uint8_t GetBeepDelayFromDistance(CarState* carState, uint8_t minDistance, bool isCorner);
        uint8_t GetBeepDurationFromDistance(CarState* carState, uint8_t minDistance, bool isCorner);
        uint8_t GetBeepDirection(uint8_t rearLeftExt, uint8_t rearRightExt, uint8_t rearLeftInt, uint8_t rearRightInt);
        uint8_t GetMinDistance(uint8_t rearLeftExt, uint8_t rearRightExt, uint8_t rearLeftInt, uint8_t rearRightInt, uint8_t *isCorner);
    public:
        static constexpr uint32_t MessageId = 0x0E1;

        BusMessage Generate(CarState* carState);
        void Parse(CarState* carState, const BusMessage& message);
};
#endif
