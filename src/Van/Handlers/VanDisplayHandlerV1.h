// VanDisplayHandler.h
#pragma once

#ifndef _VanDisplayHandler_h
    #define _VanDisplayHandler_h

#include "../Handlers/AbstractVanMessageHandler.h"

#include "../../Helpers/VanDataToBridgeToCan.h"
#include "../../Helpers/VanIgnitionDataToBridgeToCan.h"
#include "../../Helpers/DoorStatus.h"
#include "../../Helpers/VanCanDisplayPopupMap.h"

#include "../../Can/Handlers/CanTripInfoHandler.h"
#include "../../Can/Handlers/CanDisplayPopupHandler.h"
#include "../../Can/Handlers/CanStatusOfFunctionsHandler.h"
#include "../../Can/Handlers/CanWarningLogHandler.h"

#include "../Structs/VanDisplayStructsV1.h"

class VanDisplayHandlerV1 : public AbstractVanMessageHandler {
    CanDisplayPopupHandler* canPopupHandler;
    CanTripInfoHandler* canTripInfoHandler;
    CanStatusOfFunctionsHandler* canStatusOfFunctionsHandler;
    CanWarningLogHandler* canWarningLogHandler;
    VanCanDisplayPopupMap* popupMapping;
    AbstractVanMessageHandler* vanDisplayHandlerV2;

    const uint16_t LEFT_STICK_BUTTON_TIME = 5000;
    unsigned long leftStickButtonReturn = 0;
    unsigned long currentTime = 0;

    ~VanDisplayHandlerV1()
    {

    }

public:
    VanDisplayHandlerV1(
        CanDisplayPopupHandler* _canPopupHandler, 
        CanTripInfoHandler* _canTripInfoHandler, 
        VanCanDisplayPopupMap* _popupMapping,
        CanStatusOfFunctionsHandler* _canStatusOfFunctionsHandler,
        CanWarningLogHandler* _canWarningLogHandler,
        AbstractVanMessageHandler* _vanDisplayHandlerV2
        )
    {
        canPopupHandler = _canPopupHandler;
        canTripInfoHandler = _canTripInfoHandler;
        popupMapping = _popupMapping;
        canStatusOfFunctionsHandler = _canStatusOfFunctionsHandler;
        canWarningLogHandler = _canWarningLogHandler;
        vanDisplayHandlerV2 = _vanDisplayHandlerV2;
    }

    bool ProcessMessage(
        const uint8_t identByte1,
        const uint8_t identByte2,
        const uint8_t vanMessageWithoutId[],
        const uint8_t messageLength,
        VanDataToBridgeToCan& dataToBridge,
        VanIgnitionDataToBridgeToCan& ignitionDataToBridge,
        DoorStatus& doorStatus) override
    {
        if (!(IsVanIdent(identByte1, identByte2, VAN_ID_DISPLAY_POPUP_V1) && messageLength == 14))
        {
            return false;
        }

        currentTime = millis();

        uint8_t vanMessageV2[16] = { 0x00 };
        memcpy(vanMessageV2, vanMessageWithoutId, 14);

        return vanDisplayHandlerV2->ProcessMessage(identByte1, identByte2, vanMessageV2, 16, dataToBridge, ignitionDataToBridge, doorStatus);
    }
};

#endif
