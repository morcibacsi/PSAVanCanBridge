// VanMessageWriterBase.h
#pragma once

#ifndef _VanMessageWriterBase_h
    #define _VanMessageWriterBase_h

#include "../IVanMessageSender.h"

class VanMessageWriterBase
{
    unsigned long _previousTime = 0;

    virtual void InternalProcess() = 0;

    protected:
    uint16_t _processInterval = 40;
    unsigned long _currentTime = 0;

    IVanMessageSender *_vanMessageSender;

    VanMessageWriterBase(IVanMessageSender *object, uint16_t interval);

    public:
    void Process(unsigned long currentTime, bool forced);
};

#endif
