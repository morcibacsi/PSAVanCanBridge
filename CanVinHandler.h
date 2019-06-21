#pragma once

#ifndef _CanVinHandler_h
    #define _CanVinHandler_h

#include "CanVinStructs.h"
#include "CanMessageSender.h"

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

    //V2C sends this VIN:  LDC943X47D1566772
    //uint8_t Vin[17] = { 0x4C, 0x44, 0x43, 0x39, 0x34, 0x33, 0x58, 0x34, 0x37, 0x44, 0x31, 0x35, 0x36, 0x36, 0x37, 0x37, 0x32 };

    //Fake V2C sends this VIN: LDC88888888888888
    uint8_t Vin[17] = { 0x4C, 0x44, 0x43, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38 };

    //My VIN: VF33HNFUB82486672
    //uint8_t Vin[17] = { 0x56, 0x46, 0x33, 0x33, 0x48, 0x4E, 0x46, 0x55, 0x42, 0x38, 0x32, 0x34, 0x38, 0x36, 0x36, 0x37, 0x32 };

    //uint8_t Vin[17] = { 0 };

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
