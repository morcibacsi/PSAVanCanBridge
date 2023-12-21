#include "VinFlashStorageEsp32.h"
#include "../ESPFlash/ESPFlash.h"
#include "DebugPrint.h"

VinFlashStorageEsp32::VinFlashStorageEsp32(Config* config)
{
    _config = config;
}

void VinFlashStorageEsp32::Remove()
{
    ESPFlash<uint8_t> vinNumber(_fileName, true);
    vinNumber.clear();
}

bool VinFlashStorageEsp32::Load()
{
    ESPFlash<uint8_t> vinNumber(_fileName, true);

    bool success = vinNumber.getFrontElements(_config->VIN_FOR_HEADUNIT, VIN_LENGTH);

    printArray(F("VIN number loaded: "), _config->VIN_FOR_HEADUNIT, VIN_LENGTH);

    return success;
}

bool VinFlashStorageEsp32::Save()
{
    ESPFlash<uint8_t> vinNumber(_fileName, true);

    bool success = vinNumber.setElements(_config->VIN_FOR_HEADUNIT, VIN_LENGTH);

    printArray(F("VIN number saved: "), _config->VIN_FOR_HEADUNIT, VIN_LENGTH);

    return success;
}
