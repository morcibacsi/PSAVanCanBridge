#include "VanSoundOptionState.h"

VanSoundOptionState::VanSoundOptionState()
{
    Reset();
}

void VanSoundOptionState::Next()
{
    switch (_currentState)
    {
        case OptionStateNone:
            _currentState = OptionStateBass;
            break;
        case OptionStateBass:
            _currentState = OptionStateTreble;
            break;
        case OptionStateTreble:
            _currentState = OptionStateLoudness;
            break;
        case OptionStateLoudness:
            _currentState = OptionStateFader;
            break;
        case OptionStateFader:
            _currentState = OptionStateBalance;
            break;
        case OptionStateBalance:
            _currentState = OptionStateAutoVol;
            break;
        case OptionStateAutoVol:
            _currentState = OptionStateNone;
            break;
    }
}

void VanSoundOptionState::Reset()
{
    _currentState = OptionStateNone;
}

void VanSoundOptionState::SetVisible(uint8_t visible)
{
    _visible = visible;
}

uint8_t VanSoundOptionState::IsVisible()
{
    return _visible;
}

OptionState VanSoundOptionState::GetState()
{
    return _currentState;
}