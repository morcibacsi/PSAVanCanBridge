#pragma once

#ifndef _MessageHandler_0E1_h
    #define _MessageHandler_0E1_h

#include "ICanMessageHandler.h"
#include "../ICanMessageSender.h"
#include "../../Helpers/DataBroker.h"
#include "../../../Config.h"
#include <inttypes.h>


//2010 is 8 byte long there are differences in byte 7
/*
    Parking radar
*/
class MessageHandler_0E1 : public ICanMessageHandler
{
    Config* _config;

    uint8_t GetBarCountFromDistance(uint8_t distanceData, bool isCorner);
    uint8_t GetBeepDelayFromDistance(uint8_t minDistance, bool isCorner);
    uint8_t GetBeepDurationFromDistance(uint8_t minDistance, bool isCorner);
    uint8_t GetBeepDirection(uint8_t rearLeftExt, uint8_t rearRightExt, uint8_t rearLeftInt, uint8_t rearRightInt);
    uint8_t GetMinDistance(uint8_t rearLeftExt, uint8_t rearRightExt, uint8_t rearLeftInt, uint8_t rearRightInt, uint8_t *isCorner);

    public:
    MessageHandler_0E1(ICanMessageSender* object, DataBroker *dataBroker, Config* config) : ICanMessageHandler(object, dataBroker, 0x0E1, 7, 100) {
        _config = config;
    };

    void SetData() override;
    uint8_t SendMessage(unsigned long currentTime, bool forcedSend) override;
};
#endif
