#pragma once

#ifndef _MessageHandler_3E5_h
    #define _MessageHandler_3E5_h

#include "ICanMessageHandler.h"
#include "../ICanMessageSender.h"
#include "../../../Config.h"
#include  "../../Helpers/DataBroker.h"
#include <inttypes.h>

enum RadioButton {
    Up    = 97,
    Down  = 98,
    Left  = 99,
    Right = 100,
    Esc   = 101,
    Ok    = 102,
    Dark  = 103,
    Menu  = 104,
    Mode  = 105,
    Trip  = 106,
    AC    = 107,
};

// 2010 completetely different

/*
    Menu control for EMF
*/
class MessageHandler_3E5 : public ICanMessageHandler
{
    Config* _config;
    uint8_t _sendOnce = 0;

    void SetupButton2004(unsigned long currentTime, RadioButton button);
    void SetupButton2010(unsigned long currentTime, RadioButton button);

    public:
    MessageHandler_3E5(ICanMessageSender* object, DataBroker *dataBroker, Config* config) : ICanMessageHandler(object, dataBroker, 0x3E5, 6, 500) { _config = config; };

    void SendButton(unsigned long currentTime, RadioButton button);
    uint8_t SendMessage(unsigned long currentTime, bool forcedSend) override;
};
#endif
