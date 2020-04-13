// VanMessageWriterBase.h
#pragma once

#ifndef _VanMessageWriterBase_h
    #define _VanMessageWriterBase_h

#include "../AbstractVanMessageSender.h"

class VanMessageWriterBase
{
    unsigned long _previousTime = 0;

    virtual void InternalProcess() = 0;

    protected:
    uint16_t _processInterval = 40;
    unsigned long _currentTime = 0;

    AbstractVanMessageSender* _vanMessageSender;

    VanMessageWriterBase(AbstractVanMessageSender* object, uint16_t interval)
    {
        _processInterval = interval;
        _vanMessageSender = object;
    }

    public:
    void Process(unsigned long currentTime)
    {
        if (currentTime - _previousTime > _processInterval)
        {
            _previousTime = currentTime;
            _currentTime = currentTime;

            InternalProcess();
        }
    }
};

#endif
