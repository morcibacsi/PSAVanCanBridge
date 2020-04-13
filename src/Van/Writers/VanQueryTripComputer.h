// VanQueryTripComputer.h
#pragma once

#ifndef _VanQueryTripComputer_h
    #define _VanQueryTripComputer_h

#include "VanMessageWriterBase.h"
#include "../AbstractVanMessageSender.h"
#include "../Structs/VanCarStatusWithTripComputerStructs.h"

class VanQueryTripComputer : public VanMessageWriterBase
{
    const static uint16_t TRIP_COMPUTER_QUERY_INTERVAL = 120;
    const static uint8_t  TRIP_COMPUTER_CHANNEL = 0;

    uint8_t _ignition = 0;

    VanCarStatusPacketSender* carStatusSender;

    virtual void InternalProcess() override
    {
        if (_ignition)
        {
            carStatusSender->GetCarStatus(TRIP_COMPUTER_CHANNEL);
        }
    }

    public:
    VanQueryTripComputer(AbstractVanMessageSender* vanMessageSender) : VanMessageWriterBase(vanMessageSender, TRIP_COMPUTER_QUERY_INTERVAL)
    {
        carStatusSender = new VanCarStatusPacketSender(vanMessageSender);
        carStatusSender->GetCarStatus(TRIP_COMPUTER_CHANNEL);
    }

    void SetData(uint8_t ignition)
    {
        _ignition = ignition;
    }

};

#endif
