// VinFlashStorage.h
#pragma once

#ifndef _VinFlashStorage_h
    #define _VinFlashStorage_h

#include "../ESPFlash/ESPFlash.h"
#include "../../Config.h"

class VinFlashStorage
{
    const char* _fileName = "/vin";

    public:
    VinFlashStorage()
    {
    }

    void Remove()
    {
        ESPFlash<uint8_t> vinNumber(_fileName, true);
        vinNumber.clear();
    }

    bool Load()
    {
        ESPFlash<uint8_t> vinNumber(_fileName, true);

        bool success = vinNumber.getFrontElements(Vin, 17);

        return success;
    }

    bool Save()
    {
        ESPFlash<uint8_t> vinNumber(_fileName, true);

        bool success = vinNumber.setElements(Vin, 17);

        return success;
    }
};

#endif
