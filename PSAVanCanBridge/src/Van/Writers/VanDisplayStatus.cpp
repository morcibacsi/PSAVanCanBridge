#include "VanDisplayStatus.h"

void VanDisplayStatus::SendStatus(uint8_t resetTotals, uint8_t resetCumulative, uint8_t keepVanComfortAlive)
{
    for (int i = 0; i < SEND_RESET_COUNT; ++i)
    {
        displayStatusSender->SendStatus(SEND_STATUS_CHANNEL, resetTotals, resetCumulative, keepVanComfortAlive);
        delay(5);
    }
}

void VanDisplayStatus::TripReset()
{
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
    SendStatus(resetTotals, resetCumulative, 1);
}

void VanDisplayStatus::InternalProcess()
{
    if (_ignition)
    {
        switch (_resetState)
        {
            case 1:
                SendStatus(0, 0, 1);
                _resetState = 2;
                break;
            case 2:
                TripReset();
                _resetState = 3;
                break;
            case 3:
                SendStatus(0, 0, 1);
                _resetState = 0;
                break;

            default:
                _resetState = 0;
                break;
        }
    }
    else
    {
        if (_prevIgnition)
        {
            _vanComfortState = 1;
        }
        switch (_vanComfortState)
        {
            case 1:
                SendStatus(0, 0, 1);
                _vanComfortState = 2;
                break;
            case 2:
                SendStatus(0, 0, 0);
                _vanComfortState = 0;
                break;

            default:
                _vanComfortState = 0;
                break;
        }
    }
    _prevIgnition = _ignition;
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
            _resetState = 0;
            _tripButtonState = 0;
            _tripButtonPressedTime = 0;//to avoid resetting trip when the button is pressed repeatedly
        }
        if (tripButton == 1 && _tripButtonState == 1 && _tripButtonPressedTime != 0 && currentTime - _tripButtonPressedTime > 3000)
        {
            _resetState = 1;
        }
    }
}

void VanDisplayStatus::Stop()
{
    displayStatusSender->Disable(SEND_STATUS_CHANNEL);
}