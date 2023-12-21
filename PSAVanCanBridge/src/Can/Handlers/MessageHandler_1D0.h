#pragma once

#ifndef _MessageHandler_1D0_h
    #define _MessageHandler_1D0_h

#include "ICanMessageHandler.h"
#include "../ICanMessageSender.h"
#include "../../../Config.h"
#include "../../Helpers/DataBroker.h"
#include <inttypes.h>

//2010 does not exist, look for 0x3D0 and 0x350

/*
    A/C status
*/
class MessageHandler_1D0 : public ICanMessageHandler
{
    Config* _config;

    public:
    MessageHandler_1D0(ICanMessageSender* object, DataBroker *dataBroker, Config* config) : ICanMessageHandler(object, dataBroker, 0x1D0, 7, 500)
    {
        _config = config;
    };

    void SetData() override;
    uint8_t SendMessage(unsigned long currentTime, bool forcedSend) override;
};
#endif
