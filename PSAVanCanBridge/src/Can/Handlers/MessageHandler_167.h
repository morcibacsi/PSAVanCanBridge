#pragma once

#ifndef _MessageHandler_167_h
    #define _MessageHandler_167_h

#include "ICanMessageHandler.h"
#include "../ICanMessageSender.h"
#include "../../Can/Structs/CAN_167.h"
#include "../../../Config.h"
#include "../../Helpers/DataBroker.h"
#include <inttypes.h>

//2010 almost the same as 2004 slight differences in bytes 2,7,8

/*
    EMF status to send when no real device is installed
*/
class MessageHandler_167 : public ICanMessageHandler
{
    Config* _config;

    public:
    MessageHandler_167(ICanMessageSender* object, DataBroker *dataBroker, Config* config): ICanMessageHandler(object, dataBroker, 0x167, 8, 100-10)
    {
        _config = config;

        Can2004_167Byte1 byte1;
        byte1.data.trip_data_on_odometer = ODOMETER_TRIP_INFO_NOT_MANAGED_BY_EMF;
        _data[0] = byte1.asByte;
    };
    uint8_t SendMessage(unsigned long currentTime, bool forcedSend) override;
};
#endif
