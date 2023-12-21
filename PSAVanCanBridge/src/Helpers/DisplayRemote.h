#pragma once

#ifndef _DisplayRemote_h
    #define _DisplayRemote_h

#include <stdint.h>
#include "../Can/CanMessageHandlerContainer.h"
#include "../Can/Handlers/MessageHandler_3E5.h"
#include "../../Config.h"
#include "DataBroker.h"

class DisplayRemote
{
    const uint16_t MENU_OPEN_TIME = 1500;

    CanMessageHandlerContainer *_canMessageHandlerContainer;
    MessageHandler_3E5* _messageHandler_3E5;
    DataBroker *_dataBroker;
    Config* _config;
    unsigned long _seekButtonsPressTime;
    uint8_t _prevButtonState = 0;
    uint8_t _seekButtonsPressed;

    void SendButton(unsigned long currentTime, RadioButton button);

    public:
    DisplayRemote(CanMessageHandlerContainer *canMessageHandlerContainer, DataBroker *dataBroker, Config* config);
    void Process(unsigned long currentTime);
};
#endif

