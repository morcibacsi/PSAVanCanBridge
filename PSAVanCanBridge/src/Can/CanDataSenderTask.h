#pragma once

#ifndef _CanDataSenderTask_h
    #define _CanDataSenderTask_h

#include "../Helpers/VanDataToBridgeToCan.h"
#include "../../Config.h"
#include "Handlers/CanNaviPositionHandler.h"

#ifdef SEND_AC_CHANGES_TO_DISPLAY
    #ifdef USE_NEW_AIRCON_DISPLAY_SENDER
        #include "../Can/Handlers/CanAirConOnDisplayHandler.h"
    #else
        #include "../Can/Handlers/CanAirConOnDisplayHandlerOrig.h"
    #endif
#endif

class CanDataSenderTask {
    unsigned long currentTime = 0;
    unsigned long prevRadioButtonTime = 0;

    uint16_t trip0Icon1Data = 0;
    uint16_t trip0Icon2Data = 0;
    uint16_t trip0Icon3Data = 0;

    uint16_t trip1Icon1Data = 0;
    uint16_t trip1Icon2Data = 0;
    uint16_t trip1Icon3Data = 0;

    uint16_t trip2Icon1Data = 0;
    uint16_t trip2Icon2Data = 0;
    uint16_t trip2Icon3Data = 0;

    CanSpeedAndRpmHandler* _canSpeedAndRpmHandler;
    CanTripInfoHandler* _tripInfoHandler;
    ICanDisplayPopupHandler* _canPopupHandler;
    CanRadioRemoteMessageHandler* _canRadioRemoteMessageHandler;
    CanDash2MessageHandler* _canDash2MessageHandler;
    CanDash3MessageHandler* _canDash3MessageHandler;
    CanDash4MessageHandler* _canDash4MessageHandler;
    CanRadioButtonPacketSender* _canRadioButtonSender;
    CanNaviPositionHandler* _canNaviPositionHandler;
    CanAirConOnDisplayHandler* _canAirConOnDisplayHandler;

public:
    bool SendNoRadioButtonMessage = true;

    CanDataSenderTask(
        CanSpeedAndRpmHandler* canSpeedAndRpmHandler,
        CanTripInfoHandler* tripInfoHandler,
        ICanDisplayPopupHandler* canPopupHandler,
        CanRadioRemoteMessageHandler* canRadioRemoteMessageHandler,
        CanDash2MessageHandler* canDash2MessageHandler,
        CanDash3MessageHandler* canDash3MessageHandler,
        CanDash4MessageHandler* canDash4MessageHandler,
        CanRadioButtonPacketSender* canRadioButtonSender,
        CanNaviPositionHandler* canNaviPositionHandler,
        CanAirConOnDisplayHandler* canAirConOnDisplayHandler
    )
    {
        _canSpeedAndRpmHandler = canSpeedAndRpmHandler;
        _tripInfoHandler = tripInfoHandler;
        _canPopupHandler = canPopupHandler;
        _canRadioRemoteMessageHandler = canRadioRemoteMessageHandler;
        _canDash2MessageHandler = canDash2MessageHandler;
        _canDash3MessageHandler = canDash3MessageHandler;
        _canDash4MessageHandler = canDash4MessageHandler;
        _canRadioButtonSender = canRadioButtonSender;
        _canNaviPositionHandler = canNaviPositionHandler;
        _canAirConOnDisplayHandler = canAirConOnDisplayHandler;
    }

    void SendData(VanDataToBridgeToCan dataToBridge) {
        currentTime = millis();

        #pragma  region SpeedAndRpm

        _canSpeedAndRpmHandler->SetData(dataToBridge.Speed, dataToBridge.Rpm);
        _canSpeedAndRpmHandler->Process(currentTime);

        #pragma endregion

        #pragma region TripInfo

        if (DISPLAY_MODE == 1)
        {
            trip0Icon1Data = dataToBridge.FuelLeftToPump; //the distance remaining to be travelled
            trip0Icon2Data = dataToBridge.FuelConsumption; //the current consumption
            trip0Icon3Data = dataToBridge.Trip1Distance; //the range

            trip1Icon1Data = dataToBridge.Trip1Distance;
            trip1Icon2Data = dataToBridge.Trip1Consumption;
            trip1Icon3Data = dataToBridge.Trip1Speed;

            trip2Icon1Data = dataToBridge.Trip2Distance;
            trip2Icon2Data = dataToBridge.Trip2Consumption;
            trip2Icon3Data = dataToBridge.Trip2Speed;
        }
        if (DISPLAY_MODE == 2)
        {
            trip0Icon1Data = round(FUEL_TANK_CAPACITY_IN_LITERS * dataToBridge.FuelLevel / 100);
            trip0Icon2Data = dataToBridge.FuelConsumption; //the current consumption
            trip0Icon3Data = dataToBridge.Speed;

            trip1Icon1Data = dataToBridge.Trip1Distance;
            trip1Icon2Data = dataToBridge.Trip1Consumption;
            trip1Icon3Data = dataToBridge.Trip1Speed;

            trip2Icon1Data = dataToBridge.Rpm;
            trip2Icon2Data = dataToBridge.FuelConsumption;
            trip2Icon3Data = dataToBridge.Speed;

            if (dataToBridge.LeftStickButtonPressed)
            {
                trip0Icon1Data = dataToBridge.FuelLevel;
                trip0Icon3Data = dataToBridge.OilTemperature;
            }
        }
        _tripInfoHandler->SetTripData(
            trip2Icon1Data,
            trip0Icon3Data,
            trip1Icon1Data,
            trip1Icon3Data,
            trip1Icon2Data,
            trip2Icon1Data,
            trip2Icon3Data,
            trip2Icon2Data,
            trip0Icon2Data,
            trip0Icon1Data
        );
        _tripInfoHandler->Process(currentTime);

        #pragma endregion

        #pragma region PopupMessage

        if (dataToBridge.Rpm > 500) {
            _canPopupHandler->SetEngineRunning(true);
        }
        else
        {
            _canPopupHandler->SetEngineRunning(false);
        }

        _canPopupHandler->Process(currentTime);

        #pragma endregion

        #pragma region Radio remote

        _canRadioRemoteMessageHandler->Process(currentTime);

        #pragma endregion

        #pragma region AirCon

        #ifdef SEND_AC_CHANGES_TO_DISPLAY

            if (!_canPopupHandler->IsPopupVisible())
            {
                #ifdef USE_NEW_AIRCON_DISPLAY_SENDER
                canAirConOnDisplayHandler->SetData(
                    dataToBridge.InternalTemperature,
                    dataToBridge.InternalTemperature,
                    0,
                    0, // auto mode
                    dataToBridge.IsHeatingPanelPoweredOn == 1 && dataToBridge.IsAirConRunning == 0, //displays: a/c off
                    dataToBridge.IsHeatingPanelPoweredOn == 0, // displays: off
                    dataToBridge.IsWindowHeatingOn == 1, // displays: windshield icon
                    dataToBridge.AirConFanSpeed,
                    dataToBridge.IsAirRecyclingOn);
                canAirConOnDisplayHandler->Process(currentTime);
                #else
                    _canAirConOnDisplayHandler->SendCanAirConToDisplay(
                        currentTime,
                        dataToBridge.InternalTemperature,
                        dataToBridge.InternalTemperature,
                        dataToBridge.AirConDirection,
                        0, // auto mode
                        dataToBridge.IsHeatingPanelPoweredOn == 1 && dataToBridge.IsAirConEnabled == 0, //displays: a/c off
                        dataToBridge.IsHeatingPanelPoweredOn == 0, // displays: off
                        dataToBridge.IsWindowHeatingOn == 1, // displays: windshield icon
                        dataToBridge.AirConFanSpeed,
                        dataToBridge.IsAirRecyclingOn);
                #endif
            }
        #endif

        #pragma endregion

        #pragma region Lights

        _canDash2MessageHandler->SetData(
            dataToBridge.LightStatuses,
            dataToBridge.DashIcons1Field,
            dataToBridge.Ignition,
            dataToBridge.GearboxMode,
            dataToBridge.GearboxSelection,
            dataToBridge.GearboxSelection
        );
        _canDash2MessageHandler->Process(currentTime);

        #pragma endregion

        #pragma region Dash icons

        _canDash3MessageHandler->SetData(dataToBridge.DashIcons1Field);
        _canDash3MessageHandler->Process(currentTime);

        _canDash4MessageHandler->SetData(dataToBridge.FuelLevel, dataToBridge.OilTemperature);
        _canDash4MessageHandler->Process(currentTime);

        #pragma endregion

        #pragma region Navigation

        _canNaviPositionHandler->SetData(dataToBridge.RightWheelPosition, dataToBridge.LeftWheelPosition);
        _canNaviPositionHandler->Process(currentTime);

        #pragma endregion

        if (SendNoRadioButtonMessage)
        {
            if (currentTime - prevRadioButtonTime > 950)
            {
                prevRadioButtonTime = currentTime;
                _canRadioButtonSender->SendButtonCode(0);
            }
        }
    }
 };

#endif
