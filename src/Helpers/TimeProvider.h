#pragma once

#ifndef _TimeProvider_h
    #define _TimeProvider_h

#include "CarState.hpp"
#include <inttypes.h>
#include <memory>
#include "../../components/esp-idf-ds3231-0.9.10/include/esp-idf-ds3231.h"

class TimeProvider {

unsigned long _previousTime = 0;
uint8_t _sdaPin;
uint8_t _sclPin;
bool _started = false;

CarState* _carState;
rtc_handle_t* _rtc;

public:
    TimeProvider(uint8_t sdaPin, uint8_t sclPin, CarState* carState);

    void Start();
    bool Process(unsigned long currentTime);
    void SetDateTime(uint16_t year,  uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);
};

#endif
