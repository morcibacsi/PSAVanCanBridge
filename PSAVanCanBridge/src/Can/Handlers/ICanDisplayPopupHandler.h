// ICanDisplayPopupHandler.h
#pragma once

#ifndef _ICanDisplayPopupHandler_h
    #define _ICanDisplayPopupHandler_h

#include "../../Helpers/CanDisplayPopupItem.h"

class ICanDisplayPopupHandler
{
    public:

        virtual void QueueNewMessage(unsigned long currentTime, CanDisplayPopupItem item) = 0;

        virtual void Process(unsigned long currentTime) = 0;

        virtual void HideCurrentPopupMessage(unsigned long currentTime) = 0;

        virtual bool IsPopupVisible() = 0;

        virtual void Reset(unsigned long currentTime) = 0;

        virtual void ResetSeatBeltWarning(unsigned long currentTime) = 0;

        virtual void SetEngineRunning(bool isRunning) = 0;

        virtual void SetIgnition(unsigned long currentTime, bool isOn) = 0;
};

#endif
