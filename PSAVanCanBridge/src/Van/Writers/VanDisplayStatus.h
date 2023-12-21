// VanDisplayStatus.h
#pragma once

#ifndef _VanDisplayStatus_h
    #define _VanDisplayStatus_h

#include "Arduino.h"
#include "VanMessageWriterBase.h"
#include "../IVanMessageSender.h"
#include "../Senders/VanDisplayStatusPacketSender.h"

class VanDisplayStatus : public VanMessageWriterBase
{
    const static uint16_t SEND_STATUS_INTERVAL = 420;
    const static uint8_t SEND_RESET_COUNT = 1;

    // We use the same channel for the status as the trip computer query because otherwise it clashes somehow
    const static uint8_t  SEND_STATUS_CHANNEL = 0;

    uint8_t _tripButtonState = 0;
    uint8_t _resetTrip = 0;
    uint8_t _whichTripToReset = 0;
    uint8_t _resetSent = 0;
    unsigned long _tripButtonPressedTime = 0;
    uint8_t _ignition = 0;

    VanDisplayStatusPacketSender* displayStatusSender;

    void SendStatus(uint8_t resetTotals, uint8_t resetCumulative);

    virtual void InternalProcess() override;

    public:
    VanDisplayStatus(IVanMessageSender* vanMessageSender) : VanMessageWriterBase(vanMessageSender, SEND_STATUS_INTERVAL){
        displayStatusSender = new VanDisplayStatusPacketSender(vanMessageSender);
    };

    void SetData(uint8_t ignition, uint8_t tripButton, uint8_t whichTripToReset, unsigned long currentTime);

    void Stop();
};

#endif
