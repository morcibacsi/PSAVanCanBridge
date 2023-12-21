// CanDisplayPopupHandler3.h
#pragma once

#ifndef _CanDisplayPopupHandler3_h
    #define _CanDisplayPopupHandler3_h

#include "../ICanMessageSender.h"
#include "../Structs/CanDisplayStructs.h"
#include "../../Helpers/CanDisplayPopupItem.h"
#include "../../Helpers/DebugPrint.h"
#include "ICanDisplayPopupHandler.h"
#include "MessageHandler_1A1.h"

class CanDisplayPopupHandler3 : public ICanDisplayPopupHandler
{
    const uint8_t  CAN_POPUP_MESSAGE_SEND_COUNT = 3;
    //const uint16_t CAN_POPUP_INTERVAL = 400;
    const uint16_t CAN_POPUP_MESSAGE_MAX_DISPLAY_TIME = 6000;
    //const uint16_t MESSAGE_CHILLTIME = 24000;
    const uint16_t MESSAGE_CHILLTIME = 10;

    ICanMessageSender *canMessageSender;

    bool riskOfIceShown = false;
    bool seatbeltWarningShown = false;
    bool espActivatedShown = true; // by default ESP is activated and we want to show the message only when it was first deactivated
    bool espDeActivatedShown = false;
    bool isPopupVisible = false;
    bool isIgnitionOn = false;

    unsigned long previousRunTime = 0;
    unsigned long popupAddedToShow = 0;

    unsigned long popupMessageTime[256] = { 0 };

    CanDisplayPopupItem currentPopupMessage;
    CanDisplayPopupItem currentDoorMessage;
    bool isDoorMessageVisible = false;
    bool isNonDoorMessageVisible = false;

    AbsSer *_serialPort;
    MessageHandler_1A1* _messageHandler_1A1;

    void ShowDebugMessage(const String msg)
    {
        debug_println(msg);
    }

    public:
    CanDisplayPopupHandler3(ICanMessageSender * object, AbsSer *serialPort, MessageHandler_1A1* messageHandler_1A1)
    {
        canMessageSender = object;
        _serialPort = serialPort;
        _messageHandler_1A1 = messageHandler_1A1;

        currentPopupMessage.MessageType = CAN_POPUP_MSG_NONE;
        currentPopupMessage.Category = CAN_POPUP_MSG_SHOW_CATEGORY3;
        currentDoorMessage.DoorStatus1 = 0x00;
    }

    void QueueNewMessage(unsigned long currentTime, CanDisplayPopupItem incomingPopupMessage)
    {
        if (!isIgnitionOn)
        {
            return;
        }
        const uint8_t incomingMessageType = incomingPopupMessage.MessageType;
        const bool isIncomingDoorMessage = incomingMessageType == CAN_POPUP_MSG_DOORS_BOOT_BONNET_REAR_SCREEN_AND_FUEL_TANK_OPEN;

        if (!isIncomingDoorMessage)
        {
            ///*
            if (currentTime - popupMessageTime[incomingMessageType] > MESSAGE_CHILLTIME)
            {
                popupMessageTime[incomingMessageType] = currentTime;
            }
            else
            {
                ShowDebugMessage("Refused because of chilltime");
                return;
            }
            //*/
        }

        if ((riskOfIceShown && incomingMessageType == CAN_POPUP_MSG_RISK_OF_ICE) ||
            (seatbeltWarningShown && incomingMessageType == CAN_POPUP_MSG_FRONT_SEAT_BELTS_NOT_FASTENED) ||
            (espActivatedShown && incomingMessageType == CAN_POPUP_MSG_ESP_ON_ON_CMB && !espDeActivatedShown) ||
            (espDeActivatedShown && incomingMessageType == CAN_POPUP_MSG_ESP_SYSTEM_DEACTIVATED && !espActivatedShown)
           )
        {
            return;
        }

        if (isIncomingDoorMessage)
        {
            //ShowDebugMessage("isIncomingDoorMessage");
            const uint8_t prevDoorStatus = currentDoorMessage.DoorStatus1;
            currentDoorMessage = incomingPopupMessage;

            if (isDoorMessageVisible)
            {
                if (incomingPopupMessage.DoorStatus1 != prevDoorStatus)
                {
                    ShowDebugMessage("HidePreviousDoorMessage");
                    HideCurrentPopupMessage(currentTime);
                }
            }
        }
        else
        {
            if (incomingPopupMessage.Category != currentPopupMessage.Category ||
                incomingPopupMessage.MessageType != currentPopupMessage.MessageType)
            {
                ShowDebugMessage("HidePreviousMessage");
                HideCurrentPopupMessage(currentTime);
                isNonDoorMessageVisible = true;

                popupAddedToShow = currentTime;
                currentPopupMessage = incomingPopupMessage;
                ShowDebugMessage("Popup added");
                //
                /*
                debug_print("Time: ");
                debug_println(popupAddedToShow);
                debug_print("category: ");
                debug_println(currentPopupMessage.Category, HEX);
                debug_print("type: ");
                debug_println(currentPopupMessage.MessageType, HEX);
                //*/
            }
        }

    }

    void Process(unsigned long currentTime)
    {
        if (!isIgnitionOn)
        {
            return;
        }
        //ShowDebugMessage("Process");
        if (DoorMessageCanBeDisplayed())
        {
            if (currentDoorMessage.DoorStatus1 != 0x00)
            {
                ShowDebugMessage("ShowCurrentDoorMessage1");
                ShowPopupMessage(currentDoorMessage);
            }
        }
        else
        {
            bool shouldHideByTimeOut =
                (currentTime - popupAddedToShow) > CAN_POPUP_MESSAGE_MAX_DISPLAY_TIME
                && (!(currentPopupMessage.Category == CAN_POPUP_MSG_SHOW_CATEGORY3 && currentPopupMessage.MessageType == CAN_POPUP_MSG_NONE))
                ;
            if (shouldHideByTimeOut)
            {
                ShowDebugMessage("HideByTimeout");
                HideCurrentPopupMessage(currentTime);

                if (DoorMessageCanBeDisplayed())
                {
                    if (currentDoorMessage.DoorStatus1 != 0x00)
                    {
                        ShowDebugMessage("ShowCurrentDoorMessage2");
                        ShowPopupMessage(currentDoorMessage);
                        //ShowDebugMessage("Show2");
                    }
                }
            }
            else
            {
                if (currentPopupMessage.Category == CAN_POPUP_MSG_SHOW_CATEGORY3 && currentPopupMessage.MessageType == CAN_POPUP_MSG_NONE)
                {
                    ShowDebugMessage("HideByCategory");
                    HideCurrentPopupMessage(currentTime);
                }
                else
                {
                    isNonDoorMessageVisible = true;
                    ShowDebugMessage("ShowCurrentPopupMessage");
                    //debug_print("Elapsed: ");
                    //debug_println(currentTime - popupAddedToShow);
                    ShowPopupMessage(currentPopupMessage);
                    //ShowDebugMessage("Show1");
                }
            }
        }

    }

    void ShowPopupMessage(CanDisplayPopupItem message) {
        uint8_t byte7 = CanGetHighKmToDisplay(message.KmToDisplay);
        uint8_t byte8 = CanGetLowKmToDisplay(message.KmToDisplay);

        _messageHandler_1A1->SetData(message.Category, message.MessageType, 1, message.DoorStatus1, message.DoorStatus2, byte7, byte8);

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
        if (message.MessageType == CAN_POPUP_MSG_ESP_ON_ON_CMB)
        {
            espActivatedShown = true;
            espDeActivatedShown = false;
        }
        if (message.MessageType == CAN_POPUP_MSG_ESP_SYSTEM_DEACTIVATED)
        {
            espActivatedShown = false;
            espDeActivatedShown = true;
        }

        isPopupVisible = true;
    }

    void HideCurrentPopupMessage(unsigned long currentTime)
    {
        if (isPopupVisible)
        {
            _messageHandler_1A1->SetData(CAN_POPUP_MSG_HIDE, currentPopupMessage.MessageType, 0, 0x00, 0xFF, 0xFF, 0xFF);
            _messageHandler_1A1->SendMessage(currentTime, true);

            isPopupVisible = false;
            isNonDoorMessageVisible = false;
            isDoorMessageVisible = false;
        }
    }

    bool IsPopupVisible()
    {
        return isPopupVisible;
    }

    void Reset(unsigned long currentTime)
    {
        ShowDebugMessage("Reset");
        riskOfIceShown = false;
        if (IsPopupVisible())
        {
            HideCurrentPopupMessage(currentTime);
        }
        ResetSeatBeltWarning(currentTime);
        ResetEspActivatedShown(currentTime);
        currentPopupMessage.MessageType = CAN_POPUP_MSG_NONE;
        currentPopupMessage.Category = CAN_POPUP_MSG_SHOW_CATEGORY3;
        currentDoorMessage.DoorStatus1 = 0x00;

        for (size_t i = 0; i < 256; i++)
        {
            popupMessageTime[i] = 0;
        }
    }

    void ResetSeatBeltWarning(unsigned long currentTime)
    {
        seatbeltWarningShown = false;
        if (currentPopupMessage.MessageType == CAN_POPUP_MSG_FRONT_SEAT_BELTS_NOT_FASTENED)
        {
            HideCurrentPopupMessage(currentTime);
            currentPopupMessage.MessageType = CAN_POPUP_MSG_NONE;
            currentPopupMessage.Category = CAN_POPUP_MSG_SHOW_CATEGORY3;
        }
    }

    void ResetEspActivatedShown(unsigned long currentTime)
    {
        espActivatedShown = true;
        espDeActivatedShown = false;
    }

    void SetEngineRunning(bool isRunning)
    {
    }

    void SetIgnition(unsigned long currentTime, bool isOn)
    {
        isIgnitionOn = isOn;
        if (!isOn)
        {
            Reset(currentTime);
            //ResetSeatBeltWarning(currentTime);
            //ResetEspActivatedShown(currentTime);
        }
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
