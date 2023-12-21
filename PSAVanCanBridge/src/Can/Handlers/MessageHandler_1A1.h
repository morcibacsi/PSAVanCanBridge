#pragma once

#ifndef _MessageHandler_1A1_h
    #define _MessageHandler_1A1_h

#include "ICanMessageHandler.h"
#include "../ICanMessageSender.h"
#include "../../Helpers/DataBroker.h"
#include <inttypes.h>

//2010 there are differences in byte 3

/*
    Message displayed on EMF and MATT
*/
class MessageHandler_1A1 : public ICanMessageHandler
{
    bool _cmbEspSent = false;

    public:
    MessageHandler_1A1(ICanMessageSender* object, DataBroker *dataBroker) : ICanMessageHandler(object, dataBroker, 0x1A1, 8, 200)
    {
        _data[0] = 0x7F;
        _data[1] = 0xFF;
        _data[2] = 0x00;
        _data[3] = 0xFF;
        _data[4] = 0xFF;
        _data[5] = 0xFF;
        _data[6] = 0xFF;
        _data[7] = 0xFF;
    };

    void SetData(uint8_t category, uint8_t messageType, uint8_t show, uint8_t doorStatus1, uint8_t doorStatus2, uint8_t km1, uint8_t km2);
    uint8_t SendMessage(unsigned long currentTime, bool forcedSend) override;
};
#endif
