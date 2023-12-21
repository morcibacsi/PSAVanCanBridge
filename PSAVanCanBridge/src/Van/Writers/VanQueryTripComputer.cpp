#include "VanQueryTripComputer.h"

void VanQueryTripComputer::InternalProcess()
{
    if (_ignition)
    {
        carStatusSender->GetCarStatus(TRIP_COMPUTER_CHANNEL);
    }
}

void VanQueryTripComputer::SetData(uint8_t ignition)
{
    _ignition = ignition;
}

void VanQueryTripComputer::Stop()
{
    carStatusSender->Disable(TRIP_COMPUTER_CHANNEL);
}
