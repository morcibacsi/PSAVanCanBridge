// VanQueryTripComputer.h
#pragma once

#ifndef _VanQueryTripComputer_h
    #define _VanQueryTripComputer_h

#include "VanMessageWriterBase.h"
#include "../IVanMessageSender.h"
#include "../Senders/VanCarStatusPacketSender.h"

class VanQueryTripComputer : public VanMessageWriterBase
{
    const static uint16_t TRIP_COMPUTER_QUERY_INTERVAL = 80;
    const static uint8_t  TRIP_COMPUTER_CHANNEL = 0;

    uint8_t _ignition = 0;

    VanCarStatusPacketSender* carStatusSender;

    virtual void InternalProcess() override;

    public:

    VanQueryTripComputer(IVanMessageSender *vanMessageSender) : VanMessageWriterBase(vanMessageSender, TRIP_COMPUTER_QUERY_INTERVAL)
    {
        carStatusSender = new VanCarStatusPacketSender(vanMessageSender);
        carStatusSender->GetCarStatus(TRIP_COMPUTER_CHANNEL);
    }

    void SetData(uint8_t ignition);

    void Stop();
};

#endif
