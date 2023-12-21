#include "VanDisplayStatus.h"

void VanDisplayStatus::SendStatus(uint8_t resetTotals, uint8_t resetCumulative)
{
    for (int i = 0; i < SEND_RESET_COUNT; ++i)
    {
        displayStatusSender->SendStatus(SEND_STATUS_CHANNEL, resetTotals, resetCumulative);
        delay(5);
    }
}

void VanDisplayStatus::InternalProcess()
{
    if (_ignition)
    {
        if (_resetTrip == 1 && _resetSent == 0)
        {
            _resetTrip =  0;
            uint8_t resetTotals = 0;
            uint8_t resetCumulative = 0;
            if (_whichTripToReset == 2) //ODOMETER_TRIP_INFO_TRIP1 from CAN message with id 0x167
            {
                resetTotals = 1;
            }
            if (_whichTripToReset == 4) //ODOMETER_TRIP_INFO_TRIP2 from CAN message with id 0x167
            {
                resetCumulative = 1;
            }
            if (_whichTripToReset == 7) //ODOMETER_TRIP_INFO_NOT_MANAGED_BY_EMF from CAN message with id 0x167
            {
                resetTotals = 1;
                resetCumulative = 1;
            }
            SendStatus(resetTotals, resetCumulative);
            _resetTrip = 0;
            _resetSent = 1;
        }
        else
        {
            displayStatusSender->SendStatus(SEND_STATUS_CHANNEL, 0, 0);
        }
    }
}

void VanDisplayStatus::SetData(uint8_t ignition, uint8_t tripButton, uint8_t whichTripToReset, unsigned long currentTime)
{
    _ignition = ignition;
    if (_ignition)
    {
        _whichTripToReset = whichTripToReset;
        if (tripButton == 1 && _tripButtonState == 0)
        {
            _tripButtonState = 1;
            _tripButtonPressedTime = currentTime;
        }
        if (tripButton == 0 && _tripButtonState == 1)
        {
            _resetTrip = 0;
            _resetSent = 1;
            _tripButtonState = 0;
            _tripButtonPressedTime = 0;//to avoid resetting trip when the button is pressed repeatedly
        }
        if (tripButton == 1 && _tripButtonState == 1 && _tripButtonPressedTime != 0 && currentTime - _tripButtonPressedTime > 3000)
        {
            _resetTrip = 1;
        }
    }
}

void VanDisplayStatus::Stop()
{
    displayStatusSender->Disable(SEND_STATUS_CHANNEL);
}