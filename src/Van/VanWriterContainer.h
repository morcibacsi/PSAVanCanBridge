// VanWriterContainer.h
#pragma once

#ifndef _VanWriterContainer_h
    #define _VanWriterContainer_h

#include "../../Config.h"

#include "../Van/AbstractVanMessageSender.h"
#include "Writers/VanQueryTripComputer.h"
#include "Writers/VanQueryAirCon.h"
#include "Writers/VanQueryParkingAid.h"
#include "../Helpers/VanIgnitionDataToBridgeToCan.h"

class VanWriterContainer {
    AbstractVanMessageSender* vanInterface;
    VanQueryTripComputer* tripComputerQuery;
    VanQueryAirCon* acQuery;
    VanQueryParkingAid* parkingAidQuery;

    public:

    VanWriterContainer(AbstractVanMessageSender* VANInterface) {
        vanInterface = VANInterface;

        tripComputerQuery = new VanQueryTripComputer(vanInterface);

        if (QUERY_AC_STATUS)
        {
            acQuery = new VanQueryAirCon(vanInterface);
        }

        if(QUERY_PARKING_AID_DISTANCE)
        {
            parkingAidQuery = new VanQueryParkingAid(vanInterface);
        }
    }

    void Process(VanIgnitionDataToBridgeToCan ignitionData, unsigned long currentTime)
    {
        tripComputerQuery->SetData(ignitionData.Ignition);
        tripComputerQuery->Process(currentTime);

        if (QUERY_AC_STATUS)
        {
            acQuery->SetData(ignitionData.Ignition);
            acQuery->Process(currentTime);
        }

        if(QUERY_PARKING_AID_DISTANCE)
        {
            parkingAidQuery->SetData(ignitionData.Ignition, ignitionData.IsReverseEngaged);
            parkingAidQuery->Process(currentTime);
        }
    }
};

#endif
