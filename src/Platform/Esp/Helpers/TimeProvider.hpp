#pragma once

#ifndef _TimeProvider_h
    #define _TimeProvider_h

#include <cstdint>

#include "Helpers/CarState.hpp"
#include "Application/IDateTimeProvider.hpp"
#include "esp-idf-ds3231.h"

class TimeProvider : public IDateTimeProvider {

unsigned long _previousTime = 0;
uint8_t _sdaPin;
uint8_t _sclPin;
bool _started = false;
bool _paused = false;

CarState* _carState;
rtc_handle_t* _rtc;

public:
    TimeProvider(uint8_t sdaPin, uint8_t sclPin, CarState* carState);

    void Start();
    bool Process(unsigned long currentTime);
    void SetDateTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second) override;
    void Pause() { _paused = true; };
    void Resume() { _paused = false; };
};

#endif

