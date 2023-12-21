// GearCalculator.h
#pragma once

#ifndef _GearCalculator_h
    #define _GearCalculator_h

#include <stdint.h>
#include "../../Config.h"
#include "DataBroker.h"

class GearCalculator
{
    Config* _config;
    DataBroker* _dataBroker;
    uint16_t _tireCircumferenceInCm;
    uint8_t _gearNumber;
    uint8_t _prevGear;
    uint8_t _prevCanGear;

    private:
        uint16_t CalculateTireDiameterInMm(int R, int percentage, int width);
        uint16_t CalculateTireCircumferenceInMm(int R, int percentage, int width);
        uint8_t GetCanGear(uint8_t gear);
    public:
        GearCalculator(Config* config, DataBroker* dataBroker);
        int8_t GetCurrentGear();
};

#endif
