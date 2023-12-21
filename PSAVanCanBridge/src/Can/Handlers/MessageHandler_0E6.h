#pragma once

#ifndef _MessageHandler_0E6_h
    #define _MessageHandler_0E6_h

#include "ICanMessageHandler.h"
#include "../ICanMessageSender.h"
#include "../../Helpers/DataBroker.h"
#include "../../../Config.h"
#include <inttypes.h>

//2010 is 8 byte long there are differences in byte 1

/*
    Rear wheel speed related data
*/
class MessageHandler_0E6 : public ICanMessageHandler
{
    Config* _config;
    uint8_t chk_esp(uint8_t* buf);

    public:
    MessageHandler_0E6(ICanMessageSender* object, DataBroker *dataBroker, Config* config) : ICanMessageHandler(object, dataBroker, 0x0E6, 5, 100-20) {
        _config = config;

        if (_config->CAN_TYPE == 1)
        {
            _dataLength = 8;
            _data[5] = 0x83;
            _data[6] = 0x8C;
            _data[7] = chk_esp(_data);
        }
     };

    void SetData() override;
    uint8_t SendMessage(unsigned long currentTime, bool forcedSend) override;
};
#endif
