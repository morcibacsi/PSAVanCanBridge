// VanHandlerContainer.h
#pragma once

#ifndef _VanHandlerContainer_h
    #define _VanHandlerContainer_h

#include "../Helpers/VanDataToBridgeToCan.h"
#include "../Helpers/VanIgnitionDataToBridgeToCan.h"
#include "../Helpers/DoorStatus.h"

#include "Handlers/AbstractVanMessageHandler.h"
#include "Handlers/VanAirConditioner1Handler.h"
#include "Handlers/VanAirConditioner2Handler.h"
#include "Handlers/VanCarStatusWithTripComputerHandler.h"
#include "Handlers/VanDashboardHandler.h"
#include "Handlers/VanDisplayHandlerV1.h"
#include "Handlers/VanDisplayHandlerV2.h"
#include "Handlers/VanInstrumentClusterHandlerV1.h"
#include "Handlers/VanInstrumentClusterHandlerV2.h"
#include "Handlers/VanRadioRemoteHandler.h"
#include "Handlers/VanSpeedAndRpmHandler.h"
#include "Handlers/VanAirConditionerDiagSensorHandler.h"
#include "Handlers/VanAirConditionerDiagActuatorHandler.h"
#include "Handlers/VanParkingAidDiagDistanceHandler.h"
#include "Handlers/VanParkingAidDiagInputStateHandler.h"

class VanHandlerContainer {
    const static uint8_t VAN_MESSAGE_HANDLER_COUNT = 13;
    AbstractVanMessageHandler* vanMessageHandlers[VAN_MESSAGE_HANDLER_COUNT];

    VanCanAirConditionerSpeedMap* vanCanAirConditionerSpeedMap;
    VanCanDisplayPopupMap* popupMapping;

    public:
    VanHandlerContainer(
        ICanDisplayPopupHandler* canPopupHandler,
        CanTripInfoHandler* canTripInfoHandler,
        CanStatusOfFunctionsHandler* canStatusOfFunctionsHandler,
        CanWarningLogHandler* canWarningLogHandler,
        CanRadioRemoteMessageHandler* canRadioRemoteMessageHandler,
		CanRadioButtonPacketSender* radioButtons
    ) {
        vanCanAirConditionerSpeedMap = new VanCanAirConditionerSpeedMap();
        popupMapping = new VanCanDisplayPopupMap();

        vanMessageHandlers[0] = new VanAirConditioner1Handler(vanCanAirConditionerSpeedMap);
        vanMessageHandlers[1] = new VanAirConditioner2Handler();
        vanMessageHandlers[2] = new VanCarStatusWithTripComputerHandler(canPopupHandler, canTripInfoHandler);
        vanMessageHandlers[3] = new VanDashboardHandler();

        vanMessageHandlers[4] = new VanDisplayHandlerV2(canPopupHandler, canTripInfoHandler, popupMapping, canStatusOfFunctionsHandler, canWarningLogHandler);
        vanMessageHandlers[5] = new VanDisplayHandlerV1(vanMessageHandlers[4]);

        vanMessageHandlers[6] = new VanInstrumentClusterHandlerV2();
        vanMessageHandlers[7] = new VanInstrumentClusterHandlerV1(vanMessageHandlers[6]);

        vanMessageHandlers[8] = new VanRadioRemoteHandler(canTripInfoHandler, canRadioRemoteMessageHandler, radioButtons);
        vanMessageHandlers[9] = new VanSpeedAndRpmHandler();
        vanMessageHandlers[10] = new VanAirConditionerDiagSensorHandler();
        vanMessageHandlers[11] = new VanAirConditionerDiagActuatorHandler();
        vanMessageHandlers[12] = new VanParkingAidDiagDistanceHandler();

    }

    bool ProcessMessage(
        const uint8_t identByte1,
        const uint8_t identByte2,
        const uint8_t vanMessageWithoutId[],
        const uint8_t messageLength,
        VanDataToBridgeToCan& dataToBridge,
        VanIgnitionDataToBridgeToCan& ignitionDataToBridge,
        DoorStatus& doorStatus)
    {
        bool vanMessageHandled = false;

        for (uint8_t i = 0; i < VAN_MESSAGE_HANDLER_COUNT; i++)
        {
            vanMessageHandled = vanMessageHandlers[i]->ProcessMessage(identByte1, identByte2, vanMessageWithoutId, messageLength, dataToBridge, ignitionDataToBridge, doorStatus);
            if (vanMessageHandled)
            {
                break;
            }
        }

        return vanMessageHandled;
    }
};

#endif
