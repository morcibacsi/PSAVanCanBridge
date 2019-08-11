// CanWarningLogHandler.h
#pragma once

#ifndef _CanWarningLogHandler_h
    #define _CanWarningLogHandler_h

#include "AbstractCanMessageSender.h"
#include "CanWarningLogStructs.h"

class CanWarningLogHandler
{
    AbstractCanMessageSender *canMessageSender;

    int GearBoxFault = 0;
    int EngineFaultRepairNeeded = 0;
    int EngineFaultStopTheVehicle = 0;
    int PowerSteeringFault = 0;

    int gearboxFaultSent = 0;
    int engineFaultRepairNeededSent = 0;
    int engineFaultStopTheVehicleSent = 0;
    int powerSteeringFaultSent = 0;

    int isInited = 0;

    void SendData()
    {
        PacketGenerator<CanWarningLogPacket> generator;

        generator.packet.data.EnableLog = CAN_WARNING_LOG_ENABLE;
        generator.packet.data.WarningCategory3.GearboxFaultRepairNeeded = GearBoxFault;
        generator.packet.data.WarningCategory4.EngineFaultRepairNeeded = EngineFaultRepairNeeded;
        generator.packet.data.WarningCategory6.EngineFaultStopTheVehicle = EngineFaultStopTheVehicle;
        generator.packet.data.WarningCategory6.PowerSteeringFaulty = PowerSteeringFault;

        unsigned char *serializedPacket = generator.GetSerializedPacket();
        canMessageSender->SendMessage(CAN_ID_WARNING_LOG,0, sizeof(CanWarningLogPacket), serializedPacket);
    }

    public:
    CanWarningLogHandler(AbstractCanMessageSender * object)
    {
        canMessageSender = object;
    }

    void Reset()
    {
        GearBoxFault = 0;
        EngineFaultRepairNeeded = 0;
        EngineFaultStopTheVehicle = 0;
        PowerSteeringFault = 0;

        gearboxFaultSent = 0;
        engineFaultRepairNeededSent = 0;
        engineFaultStopTheVehicleSent = 0;
        powerSteeringFaultSent = 0;

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

    void SetGearBoxFault()
    {
        GearBoxFault = 1;
        if (gearboxFaultSent == 0)
        {
            gearboxFaultSent = 1;
            SendData();
        }
    }

    void SetEngineFaultRepairNeeded()
    {
        EngineFaultRepairNeeded = 1;
        if (engineFaultRepairNeededSent == 0)
        {
            engineFaultRepairNeededSent = 1;
            SendData();
        }
    }

    void SetEngineFaultStopTheVehicle()
    {
        EngineFaultStopTheVehicle = 1;
        if (engineFaultStopTheVehicleSent == 0)
        {
            engineFaultStopTheVehicleSent = 1;
            SendData();
        }
    }

    void SetPowerSteeringFault()
    {
        PowerSteeringFault = 1;
        if (powerSteeringFaultSent == 0)
        {
            powerSteeringFaultSent = 1;
            SendData();
        }
    }
};

#endif
