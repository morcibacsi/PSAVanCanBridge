// VanWriterContainer.h
#pragma once

#ifndef _VanWriterContainer_h
    #define _VanWriterContainer_h

#include "../../Config.h"

#include "IVanMessageSender.h"
#include "Writers/VanQueryTripComputer.h"
#include "Writers/VanQueryAirCon.h"
#include "Writers/VanQueryParkingAid.h"
#include "Writers/VanDisplayStatus.h"
#include "Writers/VanQueryRadioSettings.h"
#include "Writers/VanQueryRadioTuner.h"
#include "Writers/VanAcceptRadioEvent.h"

#include "Senders/VanRadioCommandPacketSender.h"
#include "../Helpers/DataBroker.h"
#include "Arduino.h"

typedef enum {
    EmfStatus = 0,
    CarStatusQuery,
    StationDataQuery,
    CmdSetRadioState,
    SetSource,
    RadioSettings,
    CmdSetKeyboardState,
    CmdSetAudioSettings
} VanCommandType;

struct VanCommand {
    VanCommandType Type;
    uint8_t PowerOn;
    uint8_t Active;

    uint8_t AutoVol;
    uint8_t Loudness;
    uint8_t Mute;
    uint8_t Source;

    uint8_t ExitOptions;
    uint8_t Balance;
    uint8_t Fader;
    uint8_t Bass;
    uint8_t Treble;
    uint8_t KeyboardState;
};

class VanWriterContainer {
    IVanMessageSender *vanInterface;
    VanQueryTripComputer *tripComputerQuery;
    VanQueryAirCon *acQuery;
    VanQueryParkingAid *parkingAidQuery;
    VanDisplayStatus *displayStatus;
    VanQueryRadioSettings *radioSettings;
    VanQueryRadioTuner *radioTuner;
    VanAcceptRadioEvent *acceptRadioEvent;
    VanRadioCommandPacketSender *radioCommand;

    DataBroker *_dataBroker;
    Config *_config;

    uint8_t _sendTripDataQuery = 0;

    QueueHandle_t _queue;

    public:

    VanWriterContainer(IVanMessageSender *VANInterface, Config *config, DataBroker *dataBroker);
    void Process(unsigned long currentTime);
    void SendDisplayStatus(unsigned long currentTime);
    void GetRadioSettings(unsigned long currentTime);
    void SetRadioState(uint8_t powerOn, uint8_t active, uint8_t keyboardActive, uint8_t autoVolume, uint8_t loudness, uint8_t mute);
    void SendRadioSetSource(uint8_t source);
    void GetCarStatus();
    void GetRadioSettings();
    void GetRadioStationData();
    void SetKeyboardState(uint8_t enabled);
    void SetAudioSettings(uint8_t exitOptions, uint8_t balance, uint8_t  fader, uint8_t bass, uint8_t treble);
};

#endif
