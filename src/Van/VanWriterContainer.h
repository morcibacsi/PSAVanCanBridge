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

#ifdef QUERY_AC_STATUS
    VanQueryAirCon* acQuery;
#endif

#ifdef QUERY_PARKING_AID_DISTANCE
    VanQueryParkingAid* parkingAidQuery;
#endif

    public:

    VanWriterContainer(AbstractVanMessageSender* VANInterface) {
        vanInterface = VANInterface;

        tripComputerQuery = new VanQueryTripComputer(vanInterface);

#ifdef QUERY_AC_STATUS
        acQuery = new VanQueryAirCon(vanInterface);
#endif

#ifdef QUERY_PARKING_AID_DISTANCE
        parkingAidQuery = new VanQueryParkingAid(vanInterface);
#endif
    }

    void Process(VanIgnitionDataToBridgeToCan ignitionData, unsigned long currentTime)
    {
        tripComputerQuery->SetData(ignitionData.Ignition);
        tripComputerQuery->Process(currentTime);

#ifdef QUERY_AC_STATUS
        acQuery->SetData(ignitionData.Ignition);
        acQuery->Process(currentTime);
#endif

#ifdef QUERY_PARKING_AID_DISTANCE
        parkingAidQuery->SetData(ignitionData.Ignition, ignitionData.IsReverseEngaged);
        parkingAidQuery->Process(currentTime);
#endif
    }
};

#endif
