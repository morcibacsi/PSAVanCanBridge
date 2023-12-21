#include "VanMessageWriterBase.h"

VanMessageWriterBase::VanMessageWriterBase(IVanMessageSender *object, uint16_t interval)
{
    _processInterval = interval;
    _vanMessageSender = object;
}

void VanMessageWriterBase::Process(unsigned long currentTime, bool forced)
{
    if (forced || currentTime - _previousTime > _processInterval)
    {
        _previousTime = currentTime;
        _currentTime = currentTime;

        InternalProcess();
    }
}