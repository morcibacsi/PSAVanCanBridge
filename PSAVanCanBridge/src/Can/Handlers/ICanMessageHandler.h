// ICanMessageHandler.h
#pragma once

#ifndef _ICanMessageHandler_h
    #define _ICanMessageHandler_h

#include <stdint.h>
#include "../ICanMessageSender.h"
#include "../../Helpers/DataBroker.h"

class ICanMessageHandler
{
    protected:
        uint16_t _canId;
        uint8_t _data[8];
        uint8_t _dataLength = 8;

        uint16_t _sendInterval;
        unsigned long _previousSendTime;

        ICanMessageSender *_canInterface;
        DataBroker *_dataBroker;

        ICanMessageHandler(ICanMessageSender *canInterface, DataBroker *dataBroker, uint16_t canId, uint8_t dataLength, uint16_t interval);
    public:
        virtual uint8_t SendMessage(unsigned long currentTime, bool forcedSend);
        virtual void SetData();
        uint16_t GetCanId();
};

#endif
