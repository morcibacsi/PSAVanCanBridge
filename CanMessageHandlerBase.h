// CanMessageHandlerBase.h
#pragma once

#ifndef _CanMessageHandlerBase_h
    #define _CanMessageHandlerBase_h

#include "AbstractCanMessageSender.h"

class CanMessageHandlerBase
{
    unsigned long processInterval = 40;
    unsigned long previousTime = 0;

    virtual void InternalProcess() = 0;

    protected:
    AbstractCanMessageSender *canMessageSender;
    CanMessageHandlerBase(AbstractCanMessageSender * object, int interval)
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

            InternalProcess();
        }
    }
};

#endif
