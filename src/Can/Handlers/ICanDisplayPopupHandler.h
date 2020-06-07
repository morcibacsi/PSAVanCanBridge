// ICanDisplayPopupHandler.h
#pragma once

#ifndef _ICanDisplayPopupHandler_h
    #define _ICanDisplayPopupHandler_h

#include "../../Helpers/CanDisplayPopupItem.h"

class ICanDisplayPopupHandler
{
    public:

        virtual void QueueNewMessage(CanDisplayPopupItem item) = 0;

        virtual void Process(unsigned long currentTime) = 0;

        virtual void ShowCanPopupMessage(uint8_t category, uint8_t messageType, int kmToDisplay, uint8_t doorStatus1, uint8_t doorStatus2, int counter) = 0;

        virtual void HideCanPopupMessage(uint8_t messageType, uint8_t doorStatus, int counter) = 0;

        virtual void HideCurrentPopupMessage() = 0;

        virtual bool IsPopupVisible() = 0;

        virtual void Reset() = 0;

        virtual void ResetSeatBeltWarning() = 0;

        virtual void SetEngineRunning(bool isRunning) = 0;

        virtual void SetIgnition(bool isOn) = 0;
};

#endif
