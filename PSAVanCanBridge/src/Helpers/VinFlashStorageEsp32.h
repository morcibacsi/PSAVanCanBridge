// VinFlashStorage.h
#pragma once

#ifndef _VinFlashStorage_h
    #define _VinFlashStorage_h

#include "IVinFlashStorage.h"
#include "../../Config.h"

class VinFlashStorageEsp32 : public IVinFlashStorage
{
    Config* _config;
    const char* _fileName = "/vin";

    public:
    VinFlashStorageEsp32(Config* config);

    void Remove() override;

    bool Load() override;

    bool Save() override;
};

#endif
