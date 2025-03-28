#include "VanCanAirConditionerSpeedMap.h"
#include "DebugPrint.h"
#include "../Can/Structs/CAN_1E3.h"

VanCanAirConditionerSpeedMap::VanCanAirConditionerSpeedMap()
{
}

uint8_t VanCanAirConditionerSpeedMap::GetFanSpeedFromVANByte(uint8_t vanByte, uint8_t isAcOn, uint8_t isRearWindowHeatingOn, uint8_t isRecycleOn)
{
    uint8_t result = previousSpeed;
    bool arrayFound = false;

    if (isAcOn == 1 && isRearWindowHeatingOn == 0 && (isRecycleOn == 0 || isRecycleOn == 1))
    {
        debug_println("use AcMap");
        arrayToUse = AcMap;
        arrayFound = true;
    }
    if (isAcOn == 1 && isRearWindowHeatingOn == 1 && isRecycleOn == 0)
    {
        debug_println("use AcRearWindowMap");
        arrayToUse = AcRearWindowMap;
        arrayFound = true;
    }
    if (isAcOn == 1 && isRearWindowHeatingOn == 1 && isRecycleOn == 1)
    {
        debug_println("use AcRearWindowRecycleMap");
        arrayToUse = AcRearWindowRecycleMap;
        arrayFound = true;
    }

    if (isAcOn == 0 && isRearWindowHeatingOn == 0 && (isRecycleOn == 0 || isRecycleOn == 1))
    {
        debug_println("use ecoMap");
        arrayToUse = ecoMap;
        arrayFound = true;
    }
    if (isAcOn == 0 && isRearWindowHeatingOn == 1 && isRecycleOn == 0)
    {
        debug_println("use ecoRearWindowMap");
        arrayToUse = ecoRearWindowMap;
        arrayFound = true;
    }
    if (isAcOn == 0 && isRearWindowHeatingOn == 1 && isRecycleOn == 1)
    {
        debug_println("use ecoRearWindowRecycleMap");
        arrayToUse = ecoRearWindowRecycleMap;
        arrayFound = true;
    }

    if (arrayFound)
    {
        debug_print("vanByte: ");
        debug_println(vanByte, HEX);
        for (uint8_t i = 0; i < 8; i++)
        {
            if (*(arrayToUse + i) == vanByte)
            {
                result = i;
                previousSpeed = result;
                break;
            }
        }
    }
    else
    {
        debug_println("array not found");
    }
    debug_print("result: ");
    debug_println(result, HEX);
    return result;
}

uint8_t VanCanAirConditionerSpeedMap::GetACDirection(uint8_t vanByte)
{
    uint8_t result = CAN_AIRCON_DISPLAY_DIRECTION_AUTO;
    if (vanByte > 0x00)
    {
        result = CAN_AIRCON_DISPLAY_DIRECTION_UP;
    }
    if (vanByte > 0x1D)
    {
        result = CAN_AIRCON_DISPLAY_DIRECTION_UPDOWN;
    }
    if (vanByte > 0x2E)
    {
        result = CAN_AIRCON_DISPLAY_DIRECTION_DOWN;
    }
    if (vanByte > 0x3E)
    {
        result = CAN_AIRCON_DISPLAY_DIRECTION_FRONTDOWN;
    }
    if (vanByte > 0x60)
    {
        result = CAN_AIRCON_DISPLAY_DIRECTION_FRONT;
    }
    return result;
}
