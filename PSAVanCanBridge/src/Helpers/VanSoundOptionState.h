// VanSoundOptionState.h
#pragma once

#ifndef _VanSoundOptionState_h
    #define _VanSoundOptionState_h

#include <stdint.h>

enum OptionState {
    OptionStateNone,
    OptionStateBass,
    OptionStateTreble,
    OptionStateLoudness,
    OptionStateFader,
    OptionStateBalance,
    OptionStateAutoVol
};

class VanSoundOptionState
{

    OptionState _currentState;
    uint8_t _visible;

    public:
    VanSoundOptionState();
    void Next();
    void Reset();
    void SetVisible(uint8_t visible);
    uint8_t IsVisible();
    OptionState GetState();
};

#endif
