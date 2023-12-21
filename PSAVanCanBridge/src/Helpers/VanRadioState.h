// VanRadioState.h
#pragma once

#ifndef _VanRadioState_h
    #define _VanRadioState_h

#include <stdint.h>
#include "VanSoundOptionState.h"
#include "../../Config.h"
#include "../Can/CanMessageHandlerContainer.h"
#include "../Van/VanWriterTask.h"
#include "../Van/Structs/VAN_8C4.h"
#include "../Van/Structs/VAN_554.h"

// Derived from VAN_EVENT_RADIO_BUF
enum RadioEvent {
    EventNone = 0,
    EventRadio = 1,
    EventTape = 3,
    EventCD = 6
};

enum SetRadioSource
{
    RadioSourceTuner = 1,
    RadioSourceCd = 2,
    //TODO need to check with tape unit
    RadioSourceTape = 2, // for changing the source it seems it uses 2
    RadioSourceTape3 = 3 // but in other cases 3 is found as the buffer_part
};

class VanRadioState
{
    unsigned long _prevAudioSettingButtonHandledTime;
    Config* _config;
    VanWriterTask* _vanWriterTask;
    VanSoundOptionState* _optionState;
    CanMessageHandlerContainer* _canMessageHandlerContainer;
    uint8_t _closeOptionSent = 0;
    uint8_t _turnOffKeyboard = 0;

    uint8_t PrevTapeAvailable;
    uint8_t TapeAvailable;

    uint8_t Muted;
    uint8_t PrevPowerOn;
    uint8_t PowerOn;
    uint8_t Active;
    uint8_t LoudnessOn;
    uint8_t AutoVolumeOn;

    uint8_t OptionsVisible;
    uint8_t Balance;
    uint8_t Fader;
    uint8_t Bass;
    uint8_t Treble;

    uint8_t BufferPart;
    uint8_t PrevSource;
    uint8_t CurrentSource;

    uint8_t GetIncrementedVal(uint8_t currentValue);
    uint8_t GetDecrementedVal(uint8_t currentValue);
    void SetPowerOn(uint8_t powerOn);
    void SetTapeAvailable(uint8_t tapeAvailable);
    void SetSource(uint8_t source);
    void IncrementAudioSettingValue();
    void DecrementAudioSettingValue();

    public:

    VanRadioState(Config* _config, VanWriterTask* vanWriterTask, VanSoundOptionState* optionState, CanMessageHandlerContainer *canMessageHandlerContainer);
    void Process(unsigned long currentTime);
    void RadioEvent(unsigned long currentTime, VanEventRadioStructs &radioEvent);
    void RadioSettings(unsigned long currentTime, VanRadioInfoStruct &settings);
    void Tuner(unsigned long currentTime, VanRadioTunerStruct &vanPacket);
    void CD(unsigned long currentTime, VanRadioCDStruct &vanPacket);
    void Tape(unsigned long currentTime, VanRadioTapeStruct &vanPacket);
    void StationList(unsigned long currentTime, VanRadioTapeStruct &vanPacket);
    void ShowAudioSettings(unsigned long currentTime);
};

#endif
