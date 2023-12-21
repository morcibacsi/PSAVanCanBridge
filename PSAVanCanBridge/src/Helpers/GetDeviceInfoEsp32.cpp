#include "GetDeviceInfoEsp32.h"
#include <Arduino.h>

uint16_t GetDeviceInfoEsp32::GetId()
{
    uint64_t macAddress = ESP.getEfuseMac();
    return (uint16_t)(macAddress >> 32);
}
