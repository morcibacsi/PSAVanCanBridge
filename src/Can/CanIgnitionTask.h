#pragma once

#ifndef _CanIgnitionTask.h
    #define _CanIgnitionTask.h

#include "Handlers/ICanDisplayPopupHandler.h"
#include "../Helpers/VanIgnitionDataToBridgeToCan.h"
#include "../Helpers/VanVinToBridgeToCan.h"

class CanIgnitionTask {
    uint8_t brightness = 15;
    int8_t externalTemperature = 0;

    bool reverseEngaged = false;
    bool SendNoRadioButtonMessage = true;

    CanIgnitionPacketSender* _radioIgnition;
    CanDashIgnitionPacketSender* _dashIgnition;
    CanParkingAidHandler* _canParkingAid;
    CanRadioRemoteMessageHandler* _canRadioRemoteMessageHandler;
    CanStatusOfFunctionsHandler* _canStatusOfFunctionsHandler;
    ICanDisplayPopupHandler* _canPopupHandler;
    CanWarningLogHandler* _canWarningLogHandler;
    CanVinHandler* _canVinHandler;

public:

    CanIgnitionTask(
        CanIgnitionPacketSender* radioIgnition,
        CanDashIgnitionPacketSender* dashIgnition,
        CanParkingAidHandler* canParkingAid,
        CanRadioRemoteMessageHandler* canRadioRemoteMessageHandler,
        CanStatusOfFunctionsHandler* canStatusOfFunctionsHandler,
        ICanDisplayPopupHandler* canPopupHandler,
        CanWarningLogHandler* canWarningLogHandler,
        CanVinHandler* canVinHandler
    )
    {
        _radioIgnition = radioIgnition;
        _dashIgnition = dashIgnition;
        _canParkingAid = canParkingAid;
        _canRadioRemoteMessageHandler = canRadioRemoteMessageHandler;
        _canStatusOfFunctionsHandler = canStatusOfFunctionsHandler;
        _canPopupHandler = canPopupHandler;
        _canWarningLogHandler = canWarningLogHandler;
        _canVinHandler = canVinHandler;
    }

    void SendIgnition(VanIgnitionDataToBridgeToCan dataToBridge, VanVinToBridgeToCan vinDataToBridge, unsigned long currentTime) {
        if (dataToBridge.Ignition == 0)
        {
            // we reset these when the ignition is switched off to get a clean state
            SendNoRadioButtonMessage = true;
            _canRadioRemoteMessageHandler->IsAndroidInstalled(true);

            _canPopupHandler->Reset();
            _canStatusOfFunctionsHandler->Reset();
            _canWarningLogHandler->Reset();
        }

        if (dataToBridge.DashboardLightingEnabled || dataToBridge.LowBeamOn)
        {
            brightness = 7;
        }
        else
        {
            brightness = 15;
        }

        #pragma region Ignition signal for radio

        _radioIgnition->SendIgnition(dataToBridge.EconomyModeActive, brightness, dataToBridge.DashboardLightingEnabled);

        #pragma endregion

        #pragma region Ignition signal for display

        externalTemperature = dataToBridge.OutsideTemperature;

        if (dataToBridge.LeftStickButtonPressed)
        {
            externalTemperature = (dataToBridge.InternalTemperature + 0.5);
        }

        _dashIgnition->SendIgnition(
            dataToBridge.Ignition,
            dataToBridge.WaterTemperature,
            externalTemperature,
            dataToBridge.MileageByte1,
            dataToBridge.MileageByte2,
            dataToBridge.MileageByte3,
            dataToBridge.IsReverseEngaged);

        _canPopupHandler->SetIgnition(dataToBridge.Ignition);

        if (dataToBridge.Ignition == 1 &&
            dataToBridge.OutsideTemperature <= 3 &&
            dataToBridge.OutsideTemperature >= -3 &&
            currentTime > 10000)
        {
            if (!_canPopupHandler->IsPopupVisible())
            {
                CanDisplayPopupItem item;
                item.Category = CAN_POPUP_MSG_SHOW_CATEGORY1;
                item.MessageType = CAN_POPUP_MSG_RISK_OF_ICE;
                item.DoorStatus1 = 0;
                item.DoorStatus2 = 0;
                _canPopupHandler->QueueNewMessage(item);
            }
        }

        #pragma endregion

        #pragma region VIN sending

        if (!_canVinHandler->IsVinSet())
        {
            _canVinHandler->SetVin(vinDataToBridge.Vin);
        }

        _canVinHandler->Process(currentTime);

        #pragma endregion

        #pragma region Parking aid
        reverseEngaged = dataToBridge.IsReverseEngaged;

        if (dataToBridge.IsReverseEngaged && dataToBridge.HaveDataFromParkingAid == 1)
        {
            _canParkingAid->SetData(dataToBridge.IsReverseEngaged, dataToBridge.IsTrailerPresent, dataToBridge.ExteriorRearLeftDistanceInCm, dataToBridge.ExteriorRearRightDistanceInCm, dataToBridge.InteriorRearLeftDistanceInCm, dataToBridge.InteriorRearRightDistanceInCm, currentTime);
            _canParkingAid->Process(currentTime);
        }
        #pragma endregion

        _canStatusOfFunctionsHandler->Init();
        _canWarningLogHandler->Init();
    }
 };

#endif
