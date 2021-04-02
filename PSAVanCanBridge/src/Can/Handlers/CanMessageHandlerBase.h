// CanMessageHandlerBase.h
#pragma once

#ifndef _CanMessageHandlerBase_h
    #define _CanMessageHandlerBase_h

#include "../AbstractCanMessageSender.h"

class CanMessageHandlerBase
{
    unsigned long previousTime = 0;

    virtual void InternalProcess() = 0;

    protected:
    uint16_t processInterval = 40;
    unsigned long _currentTime = 0;

    AbstractCanMessageSender *canMessageSender;
    CanMessageHandlerBase(AbstractCanMessageSender * object, uint16_t interval)
    {
        processInterval = interval;
        canMessageSender = object;
    }

    public:
    void Process(unsigned long currentTime)
    {
        if (currentTime - previousTime > processInterval)
        {
            previousTime = currentTime;
            _currentTime = currentTime;

            InternalProcess();
        }
    }
};

#endif
