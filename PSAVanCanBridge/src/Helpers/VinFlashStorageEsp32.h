// VinFlashStorage.h
#pragma once

#ifndef _VinFlashStorage_h
    #define _VinFlashStorage_h

#include "IVinFlashStorage.h"
#include "../ESPFlash/ESPFlash.h"
#include "../../Config.h"

class VinFlashStorageEsp32 : public IVinFlashStorage
{
    const char* _fileName = "/vin";

    public:
    VinFlashStorageEsp32()
    {
    }

    void Remove() override
    {
        ESPFlash<uint8_t> vinNumber(_fileName, true);
        vinNumber.clear();
    }

    bool Load() override
    {
        ESPFlash<uint8_t> vinNumber(_fileName, true);

        bool success = vinNumber.getFrontElements(Vin, 17);

        return success;
    }

    bool Save() override
    {
        ESPFlash<uint8_t> vinNumber(_fileName, true);

        bool success = vinNumber.setElements(Vin, 17);

        return success;
    }
};

#endif
