// VanDisplayStatus.h
#pragma once

#ifndef _VanDisplayStatus_h
    #define _VanDisplayStatus_h

#include "VanMessageWriterBase.h"
#include "../AbstractVanMessageSender.h"
#include "../../Van/Structs/VanDisplayStatusStructs.h"

class VanDisplayStatus : public VanMessageWriterBase
{
    const static uint16_t SEND_STATUS_INTERVAL = 420;
    const static uint8_t  SEND_STATUS_CHANNEL = 0;

    uint8_t _tripButtonState = 0;
    uint8_t _resetTrip = 0;
    uint8_t _resetSent = 0;
    unsigned long _tripButtonPressedTime = 0;
    uint8_t _ignition = 0;

    VanDisplayStatusPacketSender* displayStatusSender;

    virtual void InternalProcess() override
    {
        if (_ignition)
        {
            if (_resetTrip == 1 && _resetSent == 0)
            {
                displayStatusSender->SendTripReset(SEND_STATUS_CHANNEL);
                _resetTrip = 0;
                _resetSent = 1;
            }
            else
            {
                displayStatusSender->SendReady(SEND_STATUS_CHANNEL);
            }
        }
    }

    public:
    VanDisplayStatus(AbstractVanMessageSender* vanMessageSender) : VanMessageWriterBase(vanMessageSender, SEND_STATUS_INTERVAL)
    {
        displayStatusSender = new VanDisplayStatusPacketSender(vanMessageSender);
        displayStatusSender->SendReady(SEND_STATUS_CHANNEL);
    }

    void SetData(uint8_t ignition, uint8_t tripButton, unsigned long currentTime)
    {
        _ignition = ignition;
        if (_ignition)
        {
            if (tripButton == 1 && _tripButtonState == 0)
            {
                _tripButtonState = 1;
                _tripButtonPressedTime = currentTime;
            }
            if (tripButton == 0 && _tripButtonState == 1)
            {
                _tripButtonState = 0;
                _resetSent = 0;
                _resetTrip = 0;
            }
            if (tripButton == 1 && _tripButtonState == 1 && currentTime - _tripButtonPressedTime > 2000)
            {
                _resetTrip = 1;
            }
        }
    }

    void Stop()
    {
        displayStatusSender->Disable(SEND_STATUS_CHANNEL);
    }

};

#endif
