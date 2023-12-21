#pragma once

#ifndef _CanPinConfigHandler_h
    #define _CanPinConfigHandler_h

#include <stdint.h>
#include "../Can/Handlers/TpMessageHandler_760.h"
#include "../../Config.h"
#include "DataBroker.h"

class CanPinConfigHandler
{
    TpMessageHandler_760* _radioDiag;
    Config* _config;
    DataBroker* _dataBroker;


    bool radioEnabled = false;
    uint8_t source = 0;
    uint8_t band = 0;
    uint8_t freq1 = 0;
    uint8_t freq2 = 0;
    uint8_t menu = 0;
    uint8_t *vinKeyCombo;
    uint8_t vinKeyCombo_2004[4] = { 0x01, 0x04, 0x01, 0x04 };
    uint8_t vinKeyCombo_2010[4] = { 0x01, 0x02, 0x01, 0x02 };
    uint8_t keyComboEntered[4] = { 0x00, 0x00, 0x00, 0x00 };
    uint8_t keyComboCount = 0;
    bool readKeyCombo = false;

    public:
    CanPinConfigHandler(Config* config, TpMessageHandler_760* radioDiag, DataBroker* dataBroker);
    bool ProcessMessage(const uint16_t canId, const uint8_t length, const uint8_t canMsg[]);
};
#endif

