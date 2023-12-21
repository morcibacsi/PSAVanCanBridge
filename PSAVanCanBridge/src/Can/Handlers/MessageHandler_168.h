#pragma once

#ifndef _MessageHandler_168_h
    #define _MessageHandler_168_h

#include "ICanMessageHandler.h"
#include "../ICanMessageSender.h"
#include "../../Helpers/DataBroker.h"
#include "../../../Config.h"
#include <inttypes.h>

//2010 almost the same as 2004 sligth difference in all bytes

/*
    Fault lights on CMB (mil, abs, esp, etc.)
*/
class MessageHandler_168 : public ICanMessageHandler
{
    Config* _config;

    void SetData2004();
    void SetData2010();

    public:
    MessageHandler_168(ICanMessageSender* object, DataBroker *dataBroker, Config* config) : ICanMessageHandler(object, dataBroker, 0x168, 8, 200) {
        _config = config;
    };

    void SetData() override;
};
#endif
