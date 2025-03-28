#include "CrcStore.h"
#include <stddef.h>

CrcStore::CrcStore() {
    CrcItem item;
    item.PreviousCrc = 0;

    item.VanId = 0x4FC;
    CrcItems.push_back(item);

    item.VanId = 0x4DC;
    CrcItems.push_back(item);

    item.VanId = 0x464;
    CrcItems.push_back(item);

    item.VanId = 0x8A4;
    CrcItems.push_back(item);

    item.VanId = 0x524;
    CrcItems.push_back(item);

    item.VanId = 0x564;
    CrcItems.push_back(item);

    item.VanId = 0x744;
    CrcItems.push_back(item);

    item.VanId = 0x5E4;
    CrcItems.push_back(item);

    item.VanId = 0x4D4;
    CrcItems.push_back(item);

    item.VanId = 0xADC;
    CrcItems.push_back(item);
}

bool CrcStore::IsCrcSameAsPrevious(uint16_t vanId, uint16_t crc)
{
    for (CrcItem& crcItem : CrcItems)
    {
        if (crcItem.VanId == vanId)
        {
            if (crcItem.PreviousCrc == crc)
            {
                return true;
            }
            else
            {
                crcItem.PreviousCrc = crc;
                return false;
            }
        }
    }
    return false;
}