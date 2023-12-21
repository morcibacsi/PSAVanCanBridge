#include "CarState.h"
#include "../Van/Structs/VanDisplayPopupMessage.h"

/*
if return value is
    1: should show Activated popup
    0: should show DeActivated popup
    0xFF: shouldn't show a message
*/
uint8_t CarState::ChangeAutoLights(uint8_t messageByte, uint8_t newAutoLightsEnabled)
{
    if (messageByte == VAN_POPUP_MSG_AUTOMATIC_LIGHTING_ACTIVE && (_autoLightsEnabled == 0xFF || _autoLightsEnabled == 0))
    {
        _autoLightsEnabled = newAutoLightsEnabled;
        return 1;
    }

    if (_autoLightsEnabled == 0xFF)
    {
        _autoLightsEnabled = newAutoLightsEnabled;
        return 0xFF;
    }

    if (_autoLightsEnabled == 1 && newAutoLightsEnabled == 0)
    {
        _autoLightsEnabled = newAutoLightsEnabled;
        return 0;
    }

    _autoLightsEnabled = newAutoLightsEnabled;

    return 0xFF;
}

uint8_t CarState::ChangeAutoLock(uint8_t messageByte, uint8_t newAutoLockEnabled)
{
    if (messageByte == VAN_POPUP_MSG_DEADLOCKING_ACTIVE && (_autoLockEnabled == 0xFF || _autoLockEnabled == 0))
    {
        _autoLockEnabled = newAutoLockEnabled;
        return 1;
    }

    if (_autoLockEnabled == 0xFF)
    {
        _autoLockEnabled = newAutoLockEnabled;
        return 0xFF;
    }

    if (_autoLockEnabled == 1 && newAutoLockEnabled == 0)
    {
        _autoLockEnabled = newAutoLockEnabled;
        return 0;
    }

    _autoLockEnabled = newAutoLockEnabled;

    return 0xFF;
}

uint8_t CarState::ChangeAutoWipe(uint8_t messageByte, uint8_t newAutoWipeEnabled)
{
    if (messageByte == VAN_POPUP_MSG_AUTOMATIC_WIPING_ACTIVE && (_autoWipeEnabled == 0xFF || _autoWipeEnabled == 0))
    {
        _autoWipeEnabled = newAutoWipeEnabled;
        return 1;
    }

    if (_autoWipeEnabled == 0xFF)
    {
        _autoWipeEnabled = newAutoWipeEnabled;
        return 0xFF;
    }

    if (_autoWipeEnabled == 1 && newAutoWipeEnabled == 0)
    {
        _autoWipeEnabled = newAutoWipeEnabled;
        return 0;
    }

    _autoWipeEnabled = newAutoWipeEnabled;

    return 0xFF;
}

void CarState::Reset()
{
    _autoLockEnabled = 0xFF;
    _autoLightsEnabled = 0xFF;
    _autoWipeEnabled = 0xFF;
}
