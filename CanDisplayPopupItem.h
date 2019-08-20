// CanDisplayPopupItem.h
#pragma once

#ifndef _CanDisplayPopupItem_h
    #define _CanDisplayPopupItem_h

typedef struct {
    uint8_t Category;
    uint8_t MessageType;
    int DisplayTimeInMilliSeconds;
    uint8_t DoorStatus1;
    uint8_t DoorStatus2;
    int KmToDisplay;
    bool IsInited;
    int Counter;
    bool Visible;
    ulong SetVisibleOnDisplayTime;
    uint8_t VANByte;
} CanDisplayPopupItem;

#endif
