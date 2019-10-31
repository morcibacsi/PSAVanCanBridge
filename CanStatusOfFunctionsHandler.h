// CanStatusOfFunctionsHandler.h
#pragma once

#ifndef _CanStatusOfFunctionsHandler_h
    #define _CanStatusOfFunctionsHandler_h

#include "AbstractCanMessageSender.h"
#include "CanStatusOfFunctionsStructs.h"

class CanStatusOfFunctionsHandler
{
    AbstractCanMessageSender *canMessageSender;

    int AutomaticDoorlockingEnabled = 0;
    int AutomaticHeadlampEnabled = 0;
    int PassengerAirbagEnabled = 1;

    int doorLockingSent = 0;
    int headLampSent = 0;
    int airbagSent = 0;

    int isInited = 0;

    void SendData()
    {
        PacketGenerator<CanStatusOfFunctionsPacket> generator;

        generator.packet.data.FunctionsCategory0.AutomaticDoorLocking = 1;
        generator.packet.data.FunctionsCategory0.AutomaticDoorLockingActivated = AutomaticDoorlockingEnabled;
        generator.packet.data.FunctionsCategory0.AutomaticHeadLampLighting = 1;
        generator.packet.data.FunctionsCategory0.AutomaticHeadLampLightingActivated = AutomaticHeadlampEnabled;
        generator.packet.data.FunctionsCategory0.PassengersAirbag = 1;
        generator.packet.data.FunctionsCategory0.PassengersAirbagActivated = PassengerAirbagEnabled;

        unsigned char *serializedPacket = generator.GetSerializedPacket();
        canMessageSender->SendMessage(CAN_ID_STATUS_OF_FUNCTIONS,0, sizeof(CanStatusOfFunctionsPacket), serializedPacket);
    }

    public:
    CanStatusOfFunctionsHandler(AbstractCanMessageSender * object)
    {
        canMessageSender = object;
    }

    void Reset()
    {
        AutomaticDoorlockingEnabled = 0;
        AutomaticHeadlampEnabled = 0;
        PassengerAirbagEnabled = 1;

        doorLockingSent = 0;
        headLampSent = 0;
        airbagSent = 0;

        isInited = 0;
    }

    void Init()
    {
         if (isInited == 0)
         {
            isInited = 1;
            SendData();
         }
    }

    void SetAutomaticDoorLockingEnabled()
    {
        AutomaticDoorlockingEnabled = 1;
        if (doorLockingSent == 0)
        {
            doorLockingSent = 1;
            SendData();
        }
    }

    void SetAutomaticHeadlampEnabled()
    {
        AutomaticHeadlampEnabled = 1;
        if (headLampSent == 0)
        {
            headLampSent = 1;
            SendData();
        }
    }

    void SetAutomaticHeadlampDisabled()
    {
        AutomaticHeadlampEnabled = 0;
        SendData();
    }

    void SetPassengerAirbagDisabled()
    {
        PassengerAirbagEnabled = 0;
        if (airbagSent == 0)
        {
            airbagSent = 1;
            SendData();
        }
    }
};

#endif
