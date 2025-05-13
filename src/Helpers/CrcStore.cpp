#include "CrcStore.hpp"
#include <esp_attr.h>

CrcStore::CrcStore(std::vector<InitItem> initItems, bool automaticallyStoreNewIds)
{
    _automaticallyStoreNewIds = automaticallyStoreNewIds;
    for (const auto& item : initItems)
    {
        if (item.IgnoreCommand)
        {
            VanIdOnlyCrcMap[item.VanId] = {0, 0};
        }
        else
        {
            CrcKey key{ item.VanId, item.Command };
            CrcMap[key] = {0, 0};
        }
    }
}

//__attribute__((hot)) bool CrcStore::IsCrcSameAsPrevious(uint16_t vanId, uint8_t command, uint16_t crc)
bool IRAM_ATTR CrcStore::IsCrcSameAsPrevious(uint16_t vanId, uint8_t command, uint16_t crc, uint64_t currentTime)
{
    CrcKey key{ vanId, command };

    auto it = CrcMap.find(key);
    if (it != CrcMap.end())
    {
        if (it->second.crc == crc)
        {
            if (currentTime - it->second.timestamp >= CRC_TIMEOUT)
            {
                it->second.timestamp = currentTime;
                return false;
            }

            return true;
        }
        else
        {
            it->second = { crc, currentTime };
        }
        return false;
    }

    auto ignoreIt = VanIdOnlyCrcMap.find(vanId);
    if (ignoreIt != VanIdOnlyCrcMap.end())
    {
        if (ignoreIt->second.crc == crc)
        {
            if (currentTime - ignoreIt->second.timestamp >= CRC_TIMEOUT)
            {
                ignoreIt->second.timestamp = currentTime;
                return false;
            }

            return true;
        }
        ignoreIt->second = { crc, currentTime };
        return false;
    }

    if (_automaticallyStoreNewIds)
    {
        if (VanIdOnlyCrcMap.find(vanId) != VanIdOnlyCrcMap.end())
        {
            VanIdOnlyCrcMap[vanId] = { crc, currentTime };
        }
        else
        {
            CrcMap[key] = { crc, currentTime };
        }
    }

    return false;
}
