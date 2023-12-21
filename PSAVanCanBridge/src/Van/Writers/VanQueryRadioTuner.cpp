#include "VanQueryRadioTuner.h"

void VanQueryRadioTuner::InternalProcess()
{
    if (_ignition)
    {
        sender->GetTunerData(CHANNEL);
    }
}

void VanQueryRadioTuner::QueryNow()
{
    sender->GetTunerData(CHANNEL);
}

void VanQueryRadioTuner::SetData(uint8_t ignition)
{
    _ignition = ignition;
}

void VanQueryRadioTuner::Stop()
{
    sender->Disable(CHANNEL);
}
