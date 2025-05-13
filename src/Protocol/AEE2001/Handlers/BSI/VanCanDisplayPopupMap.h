// VanCanDisplayPopupMap.h
#pragma once

#ifndef _VanCanDisplayPopupMap_h
    #define _VanCanDisplayPopupMap_h

class VanCanDisplayPopupMap
{
    private:

    uint8_t VanToCanPopupMap[256];
    uint8_t VanToCanPopupCategoryMap[256];

    void SetPopupMessageTypeMapping();
    void SetPopupMessageCategoryMapping();

    public:

    VanCanDisplayPopupMap();
    uint8_t GetCanCategoryFromVanMessage(uint8_t messageId);
    uint8_t GetCanMessageIdFromVanMessage(uint8_t messageId);

};
#endif
