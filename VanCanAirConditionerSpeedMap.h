// VanCanAirConditionerSpeedMap.h
#pragma once

#ifndef _VanCanAirConditionerSpeedMap_h
    #define _VanCanAirConditionerSpeedMap_h

class VanCanAirConditionerSpeedMap
{
    /* The reported fan speed has some very weird logic. It reports different bytes for the same value based on whether the AC is on, the rear window heating is enabled and 
        the air recycling is on. I could not figure out any formula or masking to get the real value, so I chose the brute force version: 
        I created mapping arrays and choose the correct one based on the criterias.
    */

    const uint8_t AcMap[8]                   = { 0x04, 0x05, 0x06, 0x07, 0x08, 0x0A, 0x0C, 0x0E };
    //const uint8_t AcRecycleMap[8]            = { 0x04, 0x05, 0x06, 0x07, 0x08, 0x0A, 0x0C, 0x0E };
    const uint8_t AcRearWindowMap[8]         = { 0x12, 0x13, 0x14, 0x15, 0x16, 0x18, 0x1A, 0x1C };
    const uint8_t AcRearWindowRecycleMap[8]  = { 0x18, 0x19, 0x20, 0x21, 0x22, 0x24, 0x26, 0x28 };

    const uint8_t ecoMap[8]                  = { 0x02, 0x03, 0x04, 0x05, 0x06, 0x08, 0x0A, 0x0C };
    //const uint8_t ecoRecycleMap[8]           = { 0x02, 0x03, 0x04, 0x05, 0x06, 0x08, 0x0A, 0x0C };
    const uint8_t ecoRearWindowMap[8]        = { 0x10, 0x11, 0x12, 0x13, 0x14, 0x16, 0x18, 0x1A };
    const uint8_t ecoRearWindowRecycleMap[8] = { 0x16, 0x17, 0x18, 0x19, 0x20, 0x22, 0x24, 0x26 };

    const uint8_t *arrayToUse;

    private:
    public:
        VanCanAirConditionerSpeedMap()
        {
        }

        uint8_t GetFanSpeedFromVANByte(uint8_t vanByte, uint8_t isAcOn, uint8_t isRearWindowHeatingOn, uint8_t isRecycleOn)
        {
            uint8_t result = 0;

            if (isAcOn == 1 && isRearWindowHeatingOn == 0 && (isRecycleOn == 0 || isRecycleOn == 1))
            {
                arrayToUse = AcMap;
            }
            if (isAcOn == 1 && isRearWindowHeatingOn == 1 && isRecycleOn == 0)
            {
                arrayToUse = AcRearWindowMap;
            }
            if (isAcOn == 1 && isRearWindowHeatingOn == 1 && isRecycleOn == 1)
            {
                arrayToUse = AcRearWindowRecycleMap;
            }

            if (isAcOn == 0 && isRearWindowHeatingOn == 0 && (isRecycleOn == 0 || isRecycleOn == 1))
            {
                arrayToUse = ecoMap;
            }
            if (isAcOn == 0 && isRearWindowHeatingOn == 1 && isRecycleOn == 0)
            {
                arrayToUse = ecoRearWindowMap;
            }
            if (isAcOn == 0 && isRearWindowHeatingOn == 1 && isRecycleOn == 1)
            {
                arrayToUse = ecoRearWindowRecycleMap;
            }

            for (uint8_t i = 0; i < 8; i++)
            {
                if (*(arrayToUse + i) == vanByte)
                {
                    result = i + 1;
                    break;
                }
            }

            return result;
        }
};

#endif
