#pragma once

#ifndef _CanDataSenderTask_h
    #define _CanDataSenderTask_h

#include "../../Config.h"

#include "ICanMessageSender.h"
#include "../SerialPort/AbstractSerial.h"

#include "CanMessageHandlerContainer.h"

#include "Handlers/ICanDisplayPopupHandler.h"
#include "Handlers/CanDisplayPopupHandler3.h"

#include "../Helpers/VanCanGearboxPositionMap.h"
#include "../Helpers/VanCanDisplayPopupMap.h"
#include "../Helpers/VanCanAirConditionerSpeedMap.h"
#include "../Helpers/DataBroker.h"
#include "../Helpers/VanSoundOptionState.h"
#include "../Helpers/VanRadioState.h"
#include "../Helpers/IntUnions.h"
#include "../Helpers/GearCalculator.h"
#include "../Helpers/CarState.h"
#include "../Helpers/DisplayRemote.h"

#include"../Van/VanWriterTask.h"

class CanDataSenderTask {
    ICanMessageSender *_canMessageSender;
    AbsSer *_serial;
    UInt24 _mileage;

    uint8_t vanData[34];
    uint8_t vanMessageLengthWithoutId;
    uint8_t prevRadioSettings[9];

    Config *_config;
    DataBroker *_dataBroker;
    VanSoundOptionState *_vanSoundOptionState;
    VanRadioState *_vanRadioState;
    GearCalculator *_gearCalculator;
    CarState *_carState;
    DisplayRemote *_displayRemote;

    CanMessageHandlerContainer* _canMessageHandlerContainer;

    VanCanDisplayPopupMap *_popupMapping;
    VanCanGearboxPositionMap *_vanCanGearboxPositionMap;
    VanCanAirConditionerSpeedMap *_vanCanAirConditionerSpeedMap;

    ICanDisplayPopupHandler* _canPopupHandler;

    VanWriterTask *_vanWriterTask;

    unsigned long _currentTime;
    unsigned long _prevIgnitionHandledTime;

    public:
    CanDataSenderTask(ICanMessageSender *canMessageSender,
                      Config *config,
                      DataBroker *dataBroker,
                      AbsSer *serial,
                      CanMessageHandlerContainer *canMessageHandlerContainer,
                      VanWriterTask *vanWriterTask
                      );
    void SendCanMessage(unsigned long currentTime);
    void ProcessVanMessage(unsigned long currentTime, uint8_t vanMessage[], uint8_t msgLength);
    void HandleIgnition(uint8_t ignition, uint8_t engineRunning);
    void HandleOdometer();
    void HandleRearWheelData();
    void HandleRadioRemote();
    void HandleSpeedRpm();
    void HandleAlerts();
    void HandleTripData();
    void HandleAC1_464();
    void HandleAC2_4DC();
    void HandleParkingRadarGetDistance();
    void HandleParkingRadar();
    void HandleRadioSettings();
    void HandleRadio();
    void HandleRadioCommand();
    void HandleEvent();
    void HandleVin();
    void SetBrightness(uint8_t brightness, uint8_t blackPanel);
    void ResetTripOnCMB();
    void ShutOffIgnition();
    void IgnitionPacketArrived(unsigned long currentTime);
    void SendManualGear();
    void Process(unsigned long currentTime);
};
#endif
