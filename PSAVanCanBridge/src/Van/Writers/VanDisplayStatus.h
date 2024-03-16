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
    const static uint16_t SEND_STATUS_INTERVAL = 500;
    const static uint8_t SEND_RESET_COUNT = 1;

    // We use the same channel for the status as the trip computer query because otherwise it clashes somehow
    const static uint8_t  SEND_STATUS_CHANNEL = 0;

    uint8_t _tripButtonState = 0;
    uint8_t _whichTripToReset = 0;
    unsigned long _tripButtonPressedTime = 0;
    uint8_t _ignition = 0;
    uint8_t _prevIgnition = 0;
    uint8_t _resetState = 0;
    uint8_t _vanComfortState = 0;

    VanDisplayStatusPacketSender* displayStatusSender;

    void SendStatus(uint8_t resetTotals, uint8_t resetCumulative, uint8_t keepVanComfortAlive);

    virtual void InternalProcess() override;

    public:
    VanDisplayStatus(IVanMessageSender* vanMessageSender) : VanMessageWriterBase(vanMessageSender, SEND_STATUS_INTERVAL){
        displayStatusSender = new VanDisplayStatusPacketSender(vanMessageSender);
    };

    void SetData(uint8_t ignition, uint8_t tripButton, uint8_t whichTripToReset, unsigned long currentTime);
    void TripReset();

    void Stop();
};

#endif
