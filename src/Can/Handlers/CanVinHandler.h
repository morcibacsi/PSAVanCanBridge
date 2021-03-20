#pragma once

#ifndef _CanVinHandler_h
    #define _CanVinHandler_h

#include "../Structs/CanVinStructs.h"
#include "../AbstractCanMessageSender.h"
#include "../../../config.h"

class CanVinHandler
{
    const uint16_t CAN_VIN_INTERVAL = 200;

    AbstractCanMessageSender *canMessageSender;
    CanVinPacketSender *canVinSender;

    unsigned long prevVinTime= 0;

    uint8_t vinPartToSend = 1;

    public:
    CanVinHandler(AbstractCanMessageSender * object)
    {
        canMessageSender = object;
        canVinSender = new CanVinPacketSender(canMessageSender);
    }

    void SetVin(uint8_t vinBytes[17])
    {
        if (!IsVinSet())
        {
            memcpy(Vin, vinBytes, (17 * sizeof(uint8_t)));
        }
    }

    void Process(unsigned long currentTime)
    {
        if (currentTime - prevVinTime > CAN_VIN_INTERVAL)
        {
            prevVinTime = currentTime;

            if (IsVinSet())
            {
                switch (vinPartToSend)
                {
                    case 1:
                    {
                        canVinSender->SendVinPart1(Vin);
                        break;
                    }
                    case 2:
                    {
                        canVinSender->SendVinPart2(Vin);
                        break;
                    }
                    case 3:
                    {
                        //for the radio to stop beeping it is enough to send the last part of the VIN
                        canVinSender->SendVinPart3(Vin);
                        break;
                    }
                    default:
                    {
                        vinPartToSend = 0;
                        break;
                    }
                }
                vinPartToSend++;
            }
        }
    }

    bool IsVinSet()
    {
        return Vin[0] != 0x00;
    }
};

#endif
