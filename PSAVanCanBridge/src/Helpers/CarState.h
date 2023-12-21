#pragma once

#ifndef _CarState_h
    #define _CarState_h

#include <stdint.h>

class CarState
{
    uint8_t _autoLockEnabled = 0xFF;
    uint8_t _autoLightsEnabled = 0xFF;
    uint8_t _autoWipeEnabled = 0xFF;

    public:
    uint8_t ChangeAutoLock(uint8_t messageByte, uint8_t newAutoLockEnabled);
    uint8_t ChangeAutoLights(uint8_t messageByte, uint8_t newAutoLightsEnabled);
    uint8_t ChangeAutoWipe(uint8_t messageByte, uint8_t newAutoWipeEnabled);
    void Reset();

};
#endif

