#pragma once

#ifndef _MessageHandler_1E3_h
    #define _MessageHandler_1E3_h

#include "ICanMessageHandler.h"
#include "../ICanMessageSender.h"
#include "../../../Config.h"
#include "../../Helpers/DataBroker.h"
#include <inttypes.h>

//2010 does not exist, look for 0x3D0 and 0x350

/*
    A/C status to be displayed on EMF
*/
class MessageHandler_1E3 : public ICanMessageHandler
{
    Config* _config;

    public:
    MessageHandler_1E3(ICanMessageSender* object, DataBroker *dataBroker, Config* config) : ICanMessageHandler(object, dataBroker, 0x1E3, 7, 200)
    {
        _config = config;
        _data[6] = 0xF;
    };

    void SetData() override;
    uint8_t SendMessage(unsigned long currentTime, bool forcedSend) override;
};
#endif
