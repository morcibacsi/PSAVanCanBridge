// CanDisplayPopupItem.h
#pragma once

#ifndef _CanDisplayPopupItem_h
    #define _CanDisplayPopupItem_h

typedef struct {
    uint8_t Category;
    uint8_t MessageType;
    uint32_t DisplayTimeInMilliSeconds;
    uint8_t DoorStatus1;
    uint8_t DoorStatus2;
    uint16_t KmToDisplay;
    bool IsInited;
    uint16_t Counter;
    bool Visible;
    uint32_t SetVisibleOnDisplayTime;
    uint8_t VANByte;
} CanDisplayPopupItem;

#endif
