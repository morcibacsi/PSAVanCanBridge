#pragma once

#ifndef _CrcStore_h
    #define _CrcStore_h

#include <inttypes.h>
#include <list>

struct CrcItem
{
    uint16_t VanId;
    uint16_t PreviousCrc;
};

class CrcStore
{
private:
    std::list<CrcItem> CrcItems;
public:
    CrcStore();

    bool IsCrcSameAsPrevious(uint16_t vanId, uint16_t crc);
};

#endif
