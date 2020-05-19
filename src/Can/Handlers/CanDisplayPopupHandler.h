// CanDisplayPopupHandler.h
#pragma once

#ifndef _CanDisplayPopupHandler_h
    #define _CanDisplayPopupHandler_h

#include <cppQueue.h>
#include "../AbstractCanMessageSender.h"
#include "../Structs/CanDisplayStructs.h"
#include "../../Helpers/CanDisplayPopupItem.h"
#include "../../Helpers/ByteAcceptanceHandler.h"
#include "ICanDisplayPopupHandler.h"

class CanDisplayPopupHandler : public ICanDisplayPopupHandler
{
    const int CAN_POPUP_MESSAGE_TIME = 4000;
    const int CAN_POPUP_DOOR_MESSAGE_TIME = 6500;

    const int CAN_POPUP_INTERVAL = 200;
    const uint8_t CAN_POPUP_MESSAGE_SEND_COUNT = 10;

    AbstractCanMessageSender *canMessageSender;
    CanDisplayPacketSender *displayMessageSender;

    //ByteAcceptanceHandler* byteAcceptanceHandler;

    bool riskOfIceShown = false;
    bool seatbeltWarningShown = false;
    unsigned long canDisplayPopupStartTime = 0;
    unsigned long previousCanPopupTime = millis();
    bool canPopupVisible = false;
    int popupCounter = 0;
    CanDisplayPopupItem currentPopupMessage;
    CanDisplayPopupItem lastPopupMessage;
    Queue *popupMessageQueue;
    SemaphoreHandle_t canSemaphore;

    public:
    CanDisplayPopupHandler(AbstractCanMessageSender * object)
    {
        canMessageSender = object;
        popupMessageQueue = new Queue(sizeof(CanDisplayPopupItem), 10, FIFO); // Instantiate queue for popup messages
        canSemaphore = xSemaphoreCreateMutex();
        lastPopupMessage.IsInited = false;
        //byteAcceptanceHandler = new ByteAcceptanceHandler(2);
        displayMessageSender = new CanDisplayPacketSender(canMessageSender);
    }

    void QueueNewMessage(CanDisplayPopupItem item)
    {
        if (item.Category == CAN_POPUP_MSG_DOORS_BOOT_BONNET_REAR_SCREEN_AND_FUEL_TANK_OPEN)
        {
            if (item.DoorStatus1 == 0x00)
            {
                return;
            }
            item.DisplayTimeInMilliSeconds = CAN_POPUP_DOOR_MESSAGE_TIME;
        }
        else
        {
            item.DisplayTimeInMilliSeconds = CAN_POPUP_MESSAGE_TIME;
        }

        //if (byteAcceptanceHandler->GetAcceptedByte(item.MessageType) != item.MessageType)
        //{
        //    return;
        //}
        if (riskOfIceShown && item.Category == CAN_POPUP_MSG_SHOW_CATEGORY1 && item.MessageType == CAN_POPUP_MSG_RISK_OF_ICE)
        {
            return;
        }
        if (seatbeltWarningShown && item.Category == CAN_POPUP_MSG_SHOW_CATEGORY1 && item.MessageType == CAN_POPUP_MSG_FRONT_SEAT_BELTS_NOT_FASTENED)
        {
            return;
        }

        popupCounter++;
        item.Counter = popupCounter;
        bool itemCanBeQueued = false;

        CanDisplayPopupItem lastPopupMessageInQueue;
        xSemaphoreTake(canSemaphore, portMAX_DELAY);
        bool lastPeek = popupMessageQueue->peekPrevious(&lastPopupMessageInQueue);
        xSemaphoreGive(canSemaphore);
        if (!lastPopupMessage.IsInited || (lastPopupMessage.IsInited && lastPopupMessage.Category != item.Category || lastPopupMessage.MessageType != item.MessageType))
        {
            itemCanBeQueued = true;
        }
        if (lastPeek && 
            lastPopupMessageInQueue.Category == item.Category && 
            lastPopupMessageInQueue.MessageType == item.MessageType && 
            lastPopupMessageInQueue.DoorStatus1 == item.DoorStatus1 &&
            lastPopupMessageInQueue.DoorStatus2 == item.DoorStatus2)
        {
            itemCanBeQueued = false;
        }
        if (lastPopupMessage.IsInited && lastPopupMessage.MessageType == item.MessageType)
        {
            itemCanBeQueued = false;
        }
        // if the last displayed message is a door message and a new comes in we should display the new one instead of waiting for the timeout
        if (lastPopupMessage.MessageType == CAN_POPUP_MSG_DOORS_BOOT_BONNET_REAR_SCREEN_AND_FUEL_TANK_OPEN)
        {
            xSemaphoreTake(canSemaphore, portMAX_DELAY);
            popupMessageQueue->flush();
            xSemaphoreGive(canSemaphore);
            HideCanPopupMessage(lastPopupMessage.MessageType, lastPopupMessage.DoorStatus1, lastPopupMessage.Counter);
            itemCanBeQueued = true;
        }
        if (item.MessageType == CAN_POPUP_MSG_HANDBRAKE)
        {
            xSemaphoreTake(canSemaphore, portMAX_DELAY);
            popupMessageQueue->flush();
            xSemaphoreGive(canSemaphore);
            HideCanPopupMessage(lastPopupMessage.MessageType, lastPopupMessage.DoorStatus1, lastPopupMessage.Counter);
            itemCanBeQueued = true;
        }
        if (lastPopupMessage.MessageType == CAN_POPUP_MSG_FRONT_SEAT_BELTS_NOT_FASTENED &&
            item.MessageType == CAN_POPUP_MSG_FRONT_SEAT_BELTS_NOT_FASTENED)
        {
            itemCanBeQueued = true;
        }

        if (itemCanBeQueued)
        {
            xSemaphoreTake(canSemaphore, portMAX_DELAY);
            popupMessageQueue->push(&item);
            xSemaphoreGive(canSemaphore);
        }
    }

    void Process(unsigned long currentTime)
    {
        if (currentTime - previousCanPopupTime > CAN_POPUP_INTERVAL)
        {
            previousCanPopupTime = currentTime;

            //we show every message for the amount of seconds defined in CAN_POPUP_MESSAGE_TIME
            if (lastPopupMessage.Visible && (currentTime - canDisplayPopupStartTime) > lastPopupMessage.DisplayTimeInMilliSeconds)
            {
                HideCanPopupMessage(lastPopupMessage.MessageType, lastPopupMessage.DoorStatus1, lastPopupMessage.Counter);
            }

            //if the popup is not visible and we have something in the queue then we display a popup
            if (!canPopupVisible && !popupMessageQueue->isEmpty())
            {
                xSemaphoreTake(canSemaphore, portMAX_DELAY);
                popupMessageQueue->pop(&currentPopupMessage);
                xSemaphoreGive(canSemaphore);

                //by hiding first the popup we ensure that the new popup gets displayed
                if (lastPopupMessage.IsInited)
                {
                    HideCanPopupMessage(lastPopupMessage.MessageType, lastPopupMessage.DoorStatus1, lastPopupMessage.Counter);
                }
                vTaskDelay(10 / portTICK_PERIOD_MS);
                ShowCanPopupMessage(currentPopupMessage.Category, currentPopupMessage.MessageType, currentPopupMessage.KmToDisplay, currentPopupMessage.DoorStatus1, currentPopupMessage.DoorStatus2, currentPopupMessage.Counter);
                lastPopupMessage.MessageType = currentPopupMessage.MessageType;
                lastPopupMessage.DisplayTimeInMilliSeconds = currentPopupMessage.DisplayTimeInMilliSeconds;
                lastPopupMessage.IsInited = true;
                lastPopupMessage.Counter = currentPopupMessage.Counter;
                lastPopupMessage.DoorStatus1 = currentPopupMessage.DoorStatus1;
                lastPopupMessage.DoorStatus2 = currentPopupMessage.DoorStatus2;
            }
        }
    }

    void ShowCanPopupMessage(uint8_t category, uint8_t messageType, int kmToDisplay, uint8_t doorStatus1, uint8_t doorStatus2, int counter) {
        uint8_t messageSentCount = 0;
        while (messageSentCount < CAN_POPUP_MESSAGE_SEND_COUNT)
        {
            displayMessageSender->ShowPopup(category, messageType, kmToDisplay, doorStatus1, doorStatus2);
            messageSentCount++;
            vTaskDelay(5 / portTICK_PERIOD_MS);
        }
        canDisplayPopupStartTime = millis();
        canPopupVisible = true;
        lastPopupMessage.Visible = true;
        lastPopupMessage.SetVisibleOnDisplayTime = millis();
        if (messageType == CAN_POPUP_MSG_RISK_OF_ICE)
        {
            riskOfIceShown = true;
        }
        if (messageType == CAN_POPUP_MSG_FRONT_SEAT_BELTS_NOT_FASTENED)
        {
            seatbeltWarningShown = true;
        }
    }

    void HideCanPopupMessage(uint8_t messageType, uint8_t doorStatus, int counter)
    {
        uint8_t messageSentCount = 0;
        while (messageSentCount < CAN_POPUP_MESSAGE_SEND_COUNT)
        {
            displayMessageSender->HidePopup(messageType);
            messageSentCount++;
            vTaskDelay(5 / portTICK_PERIOD_MS);
        }
        lastPopupMessage.DisplayTimeInMilliSeconds = 0;
        lastPopupMessage.Visible = false;
        canPopupVisible = false;
    }

    void HideCurrentPopupMessage()
    {
        HideCanPopupMessage(lastPopupMessage.MessageType, lastPopupMessage.DoorStatus1, lastPopupMessage.Counter);
    }

    bool IsPopupVisible()
    {
        return canPopupVisible;
    }

    void Reset()
    {
        lastPopupMessage.IsInited = false;
        xSemaphoreTake(canSemaphore, portMAX_DELAY);
        popupMessageQueue->flush();
        xSemaphoreGive(canSemaphore);
        riskOfIceShown = false;
        canDisplayPopupStartTime = 0;
        if (IsPopupVisible())
        {
            HideCurrentPopupMessage();
        }
        ResetSeatBeltWarning();
    }

    void ResetSeatBeltWarning()
    {
        seatbeltWarningShown = false;
        if (lastPopupMessage.MessageType == CAN_POPUP_MSG_FRONT_SEAT_BELTS_NOT_FASTENED &&
            IsPopupVisible()) 
        {
            HideCanPopupMessage(lastPopupMessage.MessageType, lastPopupMessage.DoorStatus1, lastPopupMessage.Counter);
        }
    }

    void SetEngineRunning(bool isRunning)
    {
    }

    void SetIgnition(bool isOn)
    {
    }
};

#endif
