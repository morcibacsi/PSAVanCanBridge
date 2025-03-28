#pragma once

#ifndef _VanWriterTask_h
    #define _VanWriterTask_h

#include "VanWriterContainer.h"
#include <SPI.h>

class VanWriterTask {
    VanWriterContainer* vanWriterContainer;
    SPIClass* spi;
    IVanMessageSender* VANInterface;

public:
    VanWriterTask(Config *config, DataBroker *dataBroker);
    void SetRadioState(unsigned long currentTime, uint8_t powerOn, uint8_t active, uint8_t keyboardActive, uint8_t autoVolume, uint8_t loudness, uint8_t mute);
    void SendRadioSetSource(unsigned long currentTime, uint8_t source);
    void QueryParkingRadarData(unsigned long currentTime);
    void QueryAirConActuatorData(unsigned long currentTime);
    void GetCarStatus(unsigned long currentTime);
    void GetRadioSettings(unsigned long currentTime);
    void GetRadioStationData(unsigned long currentTime);
    void SetKeyboardState(unsigned long currentTime, uint8_t enabled);
    void SetAudioSettings(unsigned long currentTime, uint8_t exitOptions, uint8_t  balance, uint8_t  fader, uint8_t  bass, uint8_t treble);
    void Process(unsigned long currentTime);
};

#endif