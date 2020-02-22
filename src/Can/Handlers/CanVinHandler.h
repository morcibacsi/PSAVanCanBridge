#pragma once

#ifndef _CanVinHandler_h
    #define _CanVinHandler_h

#include "../Structs/CanVinStructs.h"
#include "../AbstractCanMessageSender.h"
#include "../../../config.h"

class CanVinHandler
{
    const int CAN_VIN_INTERVAL = 455;

    AbstractCanMessageSender *canMessageSender;
    CanVinPacketSender *canVinSender;

    //unsigned long nextVin1Time = 0;
    //unsigned long nextVin2Time = 1100;
    //unsigned long nextVin3Time = 2300;

    unsigned long nextVin1Time = 0;
    unsigned long nextVin2Time = 300;
    unsigned long nextVin3Time = 600;

    SemaphoreHandle_t canSemaphore;

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
        if (IsVinSet())
        {
            if (currentTime  > nextVin1Time)
            {
                nextVin1Time = currentTime + CAN_VIN_INTERVAL;
                //for the radio it is enough to send the last part of the VIN
                //canVinSender->SendVinPart1(Vin);
            }
            if (currentTime > nextVin2Time)
            {
                nextVin2Time = currentTime + CAN_VIN_INTERVAL;
                //for the radio it is enough to send the last part of the VIN
                //canVinSender->SendVinPart2(Vin);
            }
            if (currentTime > nextVin3Time)
            {
                nextVin3Time = currentTime + CAN_VIN_INTERVAL;
                canVinSender->SendVinPart3(Vin);
            }
        }
    }

    bool IsVinSet()
    {
        return Vin[0] != 0x00;
    }
};

#endif
