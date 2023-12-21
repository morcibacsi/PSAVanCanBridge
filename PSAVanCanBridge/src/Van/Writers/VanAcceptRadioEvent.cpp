#include "VanAcceptRadioEvent.h"

void VanAcceptRadioEvent::InternalProcess()
{
    if (_ignition)
    {
        sender->Accept(CHANNEL);
    }
}

void VanAcceptRadioEvent::QueryNow()
{
    sender->Accept(CHANNEL);
}

void VanAcceptRadioEvent::SetData(uint8_t ignition)
{
    _ignition = ignition;
}

void VanAcceptRadioEvent::Stop()
{
    sender->Disable(CHANNEL);
}
