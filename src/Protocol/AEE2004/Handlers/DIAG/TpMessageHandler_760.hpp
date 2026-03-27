#pragma once

#ifndef _TpMessageHandler_760_h
    #define _TpMessageHandler_760_h

#include "../../../IsoTpFrame.hpp"
#include "../../../ITransportLayer.hpp"
#include "../../../FeedbackSignal.hpp"
#include "../../../../Helpers/CarState.hpp"
#include <stdint.h>

const uint8_t CAN_DIAG_RADIO_RD4_RD43 = 0xCC;
const uint8_t CAN_DIAG_RADIO_RD45     = 0xB0;

/*
    Diag data for the radio
*/
class TpMessageHandler_760 : public IsoTpFrame
{
    #define VIN_LENGTH 17

    CarState* _carState;

    uint8_t _radioType = CAN_DIAG_RADIO_RD4_RD43;

    const uint8_t MODE_READ  = 0x21;
    const uint8_t MODE_WRITE = 0x3B;

    const uint8_t ADDRESS_VIN_RD4_RD43 = 0xCC;
    const uint8_t ADDRESS_VIN_RD45 = 0xB0;
    const uint8_t ADDRESS_OPTIONS = 0xC0;

    FeedbackSignalCallback _feedbackSignalCallback = nullptr;

    protected:
    void InternalProcess() override;

    public:
    TpMessageHandler_760(ITransportLayer* object, CarState* carState) : IsoTpFrame(object, 0x760, 0x660, 2000)
    {
         _carState = carState;
    };

    uint8_t VinNumber[VIN_LENGTH] = { 0 };

    void SetFeedbackSignalCallback(FeedbackSignalCallback feedbackSignalCallback) { _feedbackSignalCallback = feedbackSignalCallback; }

    void ReceiveFinished(unsigned long currentTime) override;

    void EnterDiagMode();
    void KeepAlive();
    void ClearFaults();
    void GetBrightnessLevels();
    void GetVinNumber();
    void SetVinNumber(uint8_t *vinNumber);
    void GetOptions();
    void SetOptions(uint8_t *options);
    void ExitDiagMode();

    void SetRadioType(uint8_t radioType);
};
#endif
