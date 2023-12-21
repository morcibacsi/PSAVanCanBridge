#include "GearCalculator.h"
#include "DebugPrint.h"
#include "../Can/Structs/CAN_128.h"
#include "../helpers/DebugPrint.h"
#include <math.h>

GearCalculator::GearCalculator(Config* config, DataBroker* dataBroker)
{
    _config = config;
    _dataBroker = dataBroker;

    _gearNumber = 0;
    for (size_t i = 1; i < 7; i++)
    {
        if (_config->GEAR_RATIOS[i] != 0)
        {
            _gearNumber++;
        }
    }

    _tireCircumferenceInCm = CalculateTireCircumferenceInMm(config->TIRE_SIZE, config->TIRE_RATIO, config->TIRE_WIDTH)  / 10;

    if (_tireCircumferenceInCm == 0)
    {
        _tireCircumferenceInCm = 1;
    }
}

int8_t GearCalculator::GetCurrentGear()
{
    uint8_t gear = 0;
    if (_dataBroker->IsReverseEngaged)
    {
        return CAN_DASH_GEAR_R;
    }
    else
    {
        uint16_t rpm = _dataBroker->Rpm.asUint16 * 0.125;
        uint16_t speed = _dataBroker->Speed.asUint16 / 100;

        //debug_println("Speed: " + String(speed));
        //debug_println("Rpm: " + String(rpm));
        //debug_println("tireCircumferenceInCm: " + String(_tireCircumferenceInCm));

        if(speed == 0 || rpm == 0)
        {
            if (_dataBroker->Handbrake)
            {
                return CAN_DASH_GEAR_P;
            }
            else
            {
                return CAN_DASH_GEAR_N;
            }

        }

        // https://github.com/zkemble/Car-Rev-Alarm/blob/master/firmware/revAlarm/revAlarm.ino
    	// TODO 1666.66 is so we use kilometers per hour instead of cm per minute??
    	// I can't remember how I got this value lol

    	float currentGearRatio = (rpm / (((1666.66f / _tireCircumferenceInCm) * _config->GEAR_RATIOS[0]) * speed));

        // Need to allow a larger difference at lower speeds since the speed is
        // in whole kph numbers and doesn't change much, while the RPM can change a lot

        //debug_println("CurrentGearRatio: " + String(currentGearRatio));
        //debug_println("CurrentGearRatio: " + String(currentGearRatio));
        //debug_println("CurrentGearRatio: " + String(currentGearRatio));
        /*
        for (size_t i = 1; i < 7; i++)
        {

            if (_config->GEAR_RATIOS[i] == 0)
            {
                continue;
            }
            //float GEAR_RATIOS[7] = { 4.53, 3.454, 1.869, 1.28, 0.95, 0.8, 0 };
            //ezen még gondolkodni kell
            if (currentGearRatio > _config->GEAR_RATIOS[i])
            {
                gear = i;
                break;
            }
        }
        */
        ///*
        float allowableRange;
        if(speed < 26)
            allowableRange = (((float)speed + 1) / (float)speed) + 0.01f;
        else
            allowableRange = 1.05f; // 5%

        while(1)
        {
            if(gear >= _gearNumber)
            {
                gear = -2;
                break;
            }

            float diff = _config->GEAR_RATIOS[gear] / currentGearRatio;

            //debug_print("currentGearRatio: ");
            //debug_println(currentGearRatio);
            //debug_print("diff: ");
            //debug_println(diff);

            gear++;

            if(diff < allowableRange && diff > 2 - allowableRange)
                break;
        }
/*
        debug_print("Current gear:");
        debug_println(gear);
        debug_print("Current gear:");
        debug_println(gear);
        debug_print("Current gear:");
        debug_println(gear);
        debug_println(gear);
        debug_println(gear);
        debug_println(gear);
        debug_println(gear);
        debug_println(gear);
*/

        if (gear == -2)
            gear = _prevGear;
        else
            _prevGear = gear;

        _prevCanGear = GetCanGear(gear);
        return _prevCanGear;

/*
        switch (gear)
        {
            case 0:
                return CAN_DASH_GEAR_D;
            case 1:
                return CAN_DASH_GEAR_1;
            case 2:
                return CAN_DASH_GEAR_2;
            case 3:
                return CAN_DASH_GEAR_3;
            case 4:
                return CAN_DASH_GEAR_4;
            case 5:
                return CAN_DASH_GEAR_5;
            case 6:
                return CAN_DASH_GEAR_6;

            default:
                return CAN_DASH_GEAR_D;
        }
/*
        for (size_t i = 7; i > 0; i--)
        {
            //float GEAR_RATIOS[7] = { 4.53, 3.454, 1.869, 1.28, 0.95, 0.8, 0 };
            //ezen még gondolkodni kell
            if (currentGearRatio >= _config->GEAR_RATIOS[i] * allowableRange)
            {
                gear = i + 1;
                break;
            }
        }
*/
    }
    //return gear;
}

uint16_t GearCalculator::CalculateTireDiameterInMm(int R, int percentage, int width)
{
    return (R+(2*((percentage * width / 100)/(2.54*10))))*2.54*10;
}

uint16_t GearCalculator::CalculateTireCircumferenceInMm(int R, int percentage, int width)
{
    return CalculateTireDiameterInMm(R, percentage, width)*M_PI;
}

uint8_t GearCalculator::GetCanGear(uint8_t gear)
{
    switch (gear)
    {
        case 0:
            return CAN_DASH_GEAR_D;
        case 1:
            return CAN_DASH_GEAR_1;
        case 2:
            return CAN_DASH_GEAR_2;
        case 3:
            return CAN_DASH_GEAR_3;
        case 4:
            return CAN_DASH_GEAR_4;
        case 5:
            return CAN_DASH_GEAR_5;
        case 6:
            return CAN_DASH_GEAR_6;

        default:
            return _prevCanGear;
    }
}