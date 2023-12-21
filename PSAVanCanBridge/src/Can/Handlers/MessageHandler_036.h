#pragma once

#ifndef _MessageHandler_036_h
    #define _MessageHandler_036_h

#include "ICanMessageHandler.h"
#include "../ICanMessageSender.h"
#include "../../Helpers/DataBroker.h"
#include "../../../Config.h"
#include <inttypes.h>

//2010 slight difference in almost all bytes, mostly compatible with 2004

/*
    Ignition data for EMF, radio, telematics, etc. Brightness for CMB
*/
class MessageHandler_036 : public ICanMessageHandler
{
    Config* _config;

    void SetData2004();
    void SetData2010();

    public:
    MessageHandler_036(ICanMessageSender* object, DataBroker *dataBroker, Config* config) : ICanMessageHandler(object, dataBroker, 0x036, 8, 100-60) {
        _config = config;
    };

    void SetData() override;
};
#endif
