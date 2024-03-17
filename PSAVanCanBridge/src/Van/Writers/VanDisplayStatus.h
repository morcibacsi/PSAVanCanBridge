// VanDisplayStatus.h
#pragma once

#ifndef _VanDisplayStatus_h
    #define _VanDisplayStatus_h

#include "Arduino.h"
#include "VanMessageWriterBase.h"
#include "../IVanMessageSender.h"
#include "../Senders/VanDisplayStatusPacketSender.h"

const uint8_t RESET_STATE_IDLE = 0;
const uint8_t RESET_STATE_SEND_STATUS_BEFORE_RESET = 1;
const uint8_t RESET_STATE_SEND_RESET = 2;
const uint8_t RESET_STATE_SEND_STATUS_AFTER_RESET = 3;

const uint8_t VAN_COMFORT_STATE_IDLE = 0;
const uint8_t VAN_COMFORT_SEND_ALIVE = 1;
const uint8_t VAN_COMFORT_SEND_STANDBY = 2;

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
    uint8_t _resetState = RESET_STATE_IDLE;
    uint8_t _vanComfortState = VAN_COMFORT_STATE_IDLE;

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
