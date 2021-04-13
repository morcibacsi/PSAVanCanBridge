// CanDisplayPopupHandler3.h
#pragma once

#ifndef _CanDisplayPopupHandler3_h
    #define _CanDisplayPopupHandler3_h

#include "../AbstractCanMessageSender.h"
#include "../Structs/CanDisplayStructs.h"
#include "../../Helpers/CanDisplayPopupItem.h"
#include "ICanDisplayPopupHandler.h"

class CanDisplayPopupHandler3 : public ICanDisplayPopupHandler
{
    const uint8_t  CAN_POPUP_MESSAGE_SEND_COUNT = 2;
    const uint16_t CAN_POPUP_INTERVAL = 400;
    const uint16_t CAN_POPUP_MESSAGE_MAX_DISPLAY_TIME = 6000;
    const uint16_t MESSAGE_CHILLTIME = 24000;

    AbstractCanMessageSender *canMessageSender;
    CanDisplayPacketSender *displayMessageSender;

    bool riskOfIceShown = false;
    bool seatbeltWarningShown = false;
    bool isPopupVisible = false;
    bool isIgnitionOn = false;

    unsigned long previousRunTime = millis();
    unsigned long popupAddedToShow = millis();

    unsigned long popupMessageTime[256] = { 0 };

    CanDisplayPopupItem currentPopupMessage;
    CanDisplayPopupItem currentDoorMessage;
    bool isDoorMessageVisible = false;
    bool isNonDoorMessageVisible = false;

    public:
    CanDisplayPopupHandler3(AbstractCanMessageSender * object)
    {
        canMessageSender = object;
        displayMessageSender = new CanDisplayPacketSender(canMessageSender);
        currentPopupMessage.MessageType = CAN_POPUP_MSG_NONE;
        currentPopupMessage.Category = CAN_POPUP_MSG_SHOW_CATEGORY3;
        currentDoorMessage.DoorStatus1 = 0x00;
    }

    void QueueNewMessage(CanDisplayPopupItem incomingPopupMessage)
    {
        if (!isIgnitionOn)
        {
            return;
        }

        const unsigned long currentTime = millis();

        const uint8_t incomingMessageType = incomingPopupMessage.MessageType;
        const bool isIncomingDoorMessage = incomingMessageType == CAN_POPUP_MSG_DOORS_BOOT_BONNET_REAR_SCREEN_AND_FUEL_TANK_OPEN;

        if (!isIncomingDoorMessage)
        {
            if (currentTime - popupMessageTime[incomingMessageType] > MESSAGE_CHILLTIME)
            {
                popupMessageTime[incomingMessageType] = currentTime;
            }
            else
            {
                return;
            }
        }

        if ((riskOfIceShown && incomingMessageType == CAN_POPUP_MSG_RISK_OF_ICE) ||
            (seatbeltWarningShown && incomingMessageType == CAN_POPUP_MSG_FRONT_SEAT_BELTS_NOT_FASTENED)
           )
        {
            return;
        }

        if (isIncomingDoorMessage)
        {
            const uint8_t prevDoorStatus = currentDoorMessage.DoorStatus1;
            currentDoorMessage = incomingPopupMessage;

            if (isDoorMessageVisible)
            {
                if (incomingPopupMessage.DoorStatus1 != prevDoorStatus)
                {
                    HideCurrentPopupMessage();
                }
            }
        }
        else
        {
            if (incomingPopupMessage.Category != currentPopupMessage.Category ||
                incomingPopupMessage.MessageType != currentPopupMessage.MessageType)
            {
                popupAddedToShow = currentTime;
                HideCurrentPopupMessage();
                isNonDoorMessageVisible = true;

                currentPopupMessage = incomingPopupMessage;
            }
        }

    }

    void Process(unsigned long currentTime)
    {
        if (!isIgnitionOn)
        {
            return;
        }

        if (currentTime - previousRunTime > CAN_POPUP_INTERVAL)
        {
            previousRunTime = currentTime;

            if (DoorMessageCanBeDisplayed())
            {
                if (currentDoorMessage.DoorStatus1 != 0x00)
                {
                    ShowPopupMessage(currentDoorMessage);
                }
            }
            else
            {
                bool shouldHideByTimeOut =
                    currentTime - popupAddedToShow > CAN_POPUP_MESSAGE_MAX_DISPLAY_TIME &&
                    currentPopupMessage.Category != CAN_POPUP_MSG_SHOW_CATEGORY3 &&
                    currentPopupMessage.MessageType != CAN_POPUP_MSG_NONE;

                if (shouldHideByTimeOut)
                {
                    HideCurrentPopupMessage();

                    if (DoorMessageCanBeDisplayed())
                    {
                        if (currentDoorMessage.DoorStatus1 != 0x00)
                        {
                            ShowPopupMessage(currentDoorMessage);
                        }
                    }
                }
                else
                {
                    if (currentPopupMessage.Category == CAN_POPUP_MSG_SHOW_CATEGORY3 && currentPopupMessage.MessageType == CAN_POPUP_MSG_NONE)
                    {
                        HideCurrentPopupMessage();
                    }
                    else
                    {
                        isNonDoorMessageVisible = true;
                        ShowPopupMessage(currentPopupMessage);
                    }
                }
            }
        }
    }

    void ShowPopupMessage(CanDisplayPopupItem message) {
        uint8_t messageSentCount = 0;
        while (messageSentCount < CAN_POPUP_MESSAGE_SEND_COUNT)
        {
            displayMessageSender->ShowPopup(message.Category, message.MessageType, message.KmToDisplay, message.DoorStatus1, message.DoorStatus2);
            messageSentCount++;
            vTaskDelay(5 / portTICK_PERIOD_MS);
        }

        if (message.MessageType == CAN_POPUP_MSG_DOORS_BOOT_BONNET_REAR_SCREEN_AND_FUEL_TANK_OPEN)
        {
            isDoorMessageVisible = true;
            isNonDoorMessageVisible = false;
        }
        else
        {
            isDoorMessageVisible = false;
            isNonDoorMessageVisible = true;
        }

        if (message.MessageType == CAN_POPUP_MSG_RISK_OF_ICE)
        {
            riskOfIceShown = true;
        }
        if (message.MessageType == CAN_POPUP_MSG_FRONT_SEAT_BELTS_NOT_FASTENED)
        {
            seatbeltWarningShown = true;
        }

        isPopupVisible = true;
    }

    void HideCurrentPopupMessage()
    {
        if (isPopupVisible)
        {
            uint8_t messageSentCount = 0;

            while (messageSentCount < CAN_POPUP_MESSAGE_SEND_COUNT)
            {
                displayMessageSender->HidePopup(currentPopupMessage.MessageType);
                messageSentCount++;
                vTaskDelay(5 / portTICK_PERIOD_MS);
            }
            isPopupVisible = false;
            isNonDoorMessageVisible = false;
            isDoorMessageVisible = false;
        }
    }

    bool IsPopupVisible()
    {
        return isPopupVisible;
    }

    void Reset()
    {
        riskOfIceShown = false;
        if (IsPopupVisible())
        {
            HideCurrentPopupMessage();
        }
        ResetSeatBeltWarning();
        currentPopupMessage.MessageType = CAN_POPUP_MSG_NONE;
        currentPopupMessage.Category = CAN_POPUP_MSG_SHOW_CATEGORY3;
        currentDoorMessage.DoorStatus1 = 0x00;

        for (size_t i = 0; i < 256; i++)
        {
            popupMessageTime[i] = 0;
        }
    }

    void ResetSeatBeltWarning()
    {
        seatbeltWarningShown = false;
        if (currentPopupMessage.MessageType == CAN_POPUP_MSG_FRONT_SEAT_BELTS_NOT_FASTENED)
        {
            HideCurrentPopupMessage();
            currentPopupMessage.MessageType = CAN_POPUP_MSG_NONE;
            currentPopupMessage.Category = CAN_POPUP_MSG_SHOW_CATEGORY3;
        }
    }

    void SetEngineRunning(bool isRunning)
    {
    }

    void SetIgnition(bool isOn)
    {
        isIgnitionOn = isOn;
    }

    bool DoorMessageCanBeDisplayed()
    {
        return
            !isNonDoorMessageVisible ||
            (currentPopupMessage.Category == CAN_POPUP_MSG_SHOW_CATEGORY3 &&
            currentPopupMessage.MessageType == CAN_POPUP_MSG_NONE);
    }
};

#endif
