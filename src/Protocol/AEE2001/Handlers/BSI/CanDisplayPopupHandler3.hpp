#pragma once

#ifndef _CanDisplayPopupHandler3_h
    #define _CanDisplayPopupHandler3_h

#include <cstdint>

#include "../../../AEE2004/Structs/CanDisplayStructs.h"
#include "CanDisplayPopupItem.h"

#include "../../../../Helpers/CarState.hpp"

class CanDisplayPopupHandler3
{
    private:

    const uint8_t  CAN_POPUP_MESSAGE_SEND_COUNT = 3;
    //const uint16_t CAN_POPUP_INTERVAL = 400;
    const uint16_t CAN_POPUP_MESSAGE_MAX_DISPLAY_TIME = 6000;
    //const uint16_t MESSAGE_CHILLTIME = 24000;
    const uint16_t MESSAGE_CHILLTIME = 10;
    const uint16_t POPUP_TRANSITION_PAUSE_TIME = 300; // just tested and compared to 120 and 1200 and this one worked in all cases during testing with EMF C 71.3

    CarState* _carState;

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

    CanDisplayPopupItem pendingPopupMessage;
    bool hasPendingPopupMessage = false;
    unsigned long pendingPopupReadyTime = 0;

    //void ShowDebugMessage(const String msg)
    void ShowDebugMessage(const char* msg)
    {
        //printf(msg);
        //printf("\n");
        //debug_println(msg);
    }

    void QueuePendingPopup(unsigned long currentTime, CanDisplayPopupItem message)
    {
        pendingPopupMessage = message;
        hasPendingPopupMessage = true;
        pendingPopupReadyTime = currentTime + POPUP_TRANSITION_PAUSE_TIME;
    }

    void TryShowPendingPopup(unsigned long currentTime)
    {
        if (!hasPendingPopupMessage)
        {
            return;
        }

        if (isPopupVisible)
        {
            return;
        }

        if (currentTime < pendingPopupReadyTime)
        {
            return;
        }

        if (pendingPopupMessage.MessageType == CAN_POPUP_MSG_DOORS_BOOT_BONNET_REAR_SCREEN_AND_FUEL_TANK_OPEN)
        {
            if (pendingPopupMessage.DoorStatus1 != 0x00 && DoorMessageCanBeDisplayed())
            {
                ShowDebugMessage("ShowPendingDoorPopup");
                ShowPopupMessage(pendingPopupMessage);
            }

            hasPendingPopupMessage = false;
            return;
        }

        ShowDebugMessage("ShowPendingNonDoorPopup");
        currentPopupMessage = pendingPopupMessage;
        popupAddedToShow = currentTime;
        ShowPopupMessage(currentPopupMessage);
        hasPendingPopupMessage = false;
    }

    public:
    CanDisplayPopupHandler3(CarState* carState)
    {
        _carState = carState;

        currentPopupMessage.MessageType = CAN_POPUP_MSG_NONE;
        currentPopupMessage.Category = CAN_POPUP_MSG_SHOW_CATEGORY3;

        currentDoorMessage.MessageType = CAN_POPUP_MSG_DOORS_BOOT_BONNET_REAR_SCREEN_AND_FUEL_TANK_OPEN;
        currentDoorMessage.Category = CAN_POPUP_MSG_SHOW_CATEGORY1;
        currentDoorMessage.DoorStatus1 = 0x00;
    }

    void QueueNewMessage(unsigned long currentTime, CanDisplayPopupItem incomingPopupMessage)
    {
        ShowDebugMessage("QueueNewMessage");
        if (_carState == nullptr)
        {
            ShowDebugMessage("CarState is null");
            return;
        }

        if (!_carState->Ignition)
        {
            return;
        }
        ShowDebugMessage("Ignition is on");

        const uint8_t incomingMessageType = incomingPopupMessage.MessageType;
        const bool isIncomingDoorMessage = incomingMessageType == CAN_POPUP_MSG_DOORS_BOOT_BONNET_REAR_SCREEN_AND_FUEL_TANK_OPEN;

        if (!isIncomingDoorMessage)
        {
            ///*
            if (currentTime - popupMessageTime[incomingMessageType] > MESSAGE_CHILLTIME)
            {
                ShowDebugMessage("Setting popupMessageTime");
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
            const uint8_t prevDoorStatus = currentDoorMessage.DoorStatus1;
            currentDoorMessage = incomingPopupMessage;

            if (hasPendingPopupMessage &&
                pendingPopupMessage.MessageType == CAN_POPUP_MSG_DOORS_BOOT_BONNET_REAR_SCREEN_AND_FUEL_TANK_OPEN)
            {
                pendingPopupMessage = currentDoorMessage;
            }

            if (isDoorMessageVisible)
            {
                if (incomingPopupMessage.DoorStatus1 != prevDoorStatus)
                {
                    ShowDebugMessage("HidePreviousDoorMessage");
                    HideCurrentPopupMessage(currentTime);
                    QueuePendingPopup(currentTime, currentDoorMessage);
                }
            }
        }
        else
        {
            ShowDebugMessage("isNonDoorMessage");
            if (incomingPopupMessage.Category != currentPopupMessage.Category ||
                incomingPopupMessage.MessageType != currentPopupMessage.MessageType)
            {
                if (isPopupVisible)
                {
                    ShowDebugMessage("HidePreviousMessage");
                    HideCurrentPopupMessage(currentTime);
                    QueuePendingPopup(currentTime, incomingPopupMessage);
                }
                else
                {
                    currentPopupMessage = incomingPopupMessage;
                    popupAddedToShow = currentTime;
                }

                ShowDebugMessage("Popup added");
            }
        }
    }

    void Process(unsigned long currentTime)
    {
        if (!isIgnitionOn)
        {
            return;
        }

        TryShowPendingPopup(currentTime);

        ShowDebugMessage("Process");
        if (DoorMessageCanBeDisplayed())
        {
            if (currentDoorMessage.DoorStatus1 != 0x00)
            {
                if (!isPopupVisible && !hasPendingPopupMessage)
                {
                    ShowDebugMessage("ShowCurrentDoorMessage1");
                    ShowPopupMessage(currentDoorMessage);
                }
            }
            else
            {
                if (isDoorMessageVisible)
                {
                    ShowDebugMessage("HideDoorPopupBecauseDoorStatusIsZero");
                    HideCurrentPopupMessage(currentTime);
                }
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
                        QueuePendingPopup(currentTime, currentDoorMessage);
                    }
                }
            }
            else
            {
                if (currentPopupMessage.Category == CAN_POPUP_MSG_SHOW_CATEGORY3 && currentPopupMessage.MessageType == CAN_POPUP_MSG_NONE)
                {
                    if (isPopupVisible)
                    {
                        ShowDebugMessage("HideByCategory");
                        HideCurrentPopupMessage(currentTime);
                    }
                }
                else
                {
                    if (!hasPendingPopupMessage)
                    {
                        isNonDoorMessageVisible = true;
                        ShowDebugMessage("ShowCurrentPopupMessage");
                        ShowPopupMessage(currentPopupMessage);
                    }
                }
            }
        }
    }

    void ShowPopupMessage(CanDisplayPopupItem message) {
        uint8_t byte7 = CanGetHighKmToDisplay(message.KmToDisplay);
        uint8_t byte8 = CanGetLowKmToDisplay(message.KmToDisplay);

        CanDisplayByte2Struct byte3{};
        byte3.data.show_popup_on_emf = 1;
        byte3.data.show_popup_on_cmb = 1;
        byte3.data.show_popup_on_vth = 1;
        byte3.data.priority = 1;

        _carState->DisplayMessage.data.Field1 = message.Category;
        _carState->DisplayMessage.data.Field2 = message.MessageType;
        _carState->DisplayMessage.data.Field3 = byte3.asByte;
        _carState->DisplayMessage.data.Field4 = message.DoorStatus1;
        _carState->DisplayMessage.data.Field5 = message.DoorStatus2;
        _carState->DisplayMessage.data.Field6 = 0xFF;
        _carState->DisplayMessage.data.Field7 = byte7;
        _carState->DisplayMessage.data.Field8 = byte8;

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
            CanDisplayByte2Struct byte3{};
            byte3.data.show_popup_on_emf = 0;
            byte3.data.show_popup_on_cmb = 0;
            byte3.data.show_popup_on_vth = 0;
            byte3.data.priority = 1;

            uint8_t popupTypeToHide = currentPopupMessage.MessageType;
            if (isDoorMessageVisible)
            {
                popupTypeToHide = currentDoorMessage.MessageType;
            }

            _carState->DisplayMessage.data.Field1 = CAN_POPUP_MSG_HIDE;
            _carState->DisplayMessage.data.Field2 = popupTypeToHide;
            _carState->DisplayMessage.data.Field3 = byte3.asByte;
            _carState->DisplayMessage.data.Field4 = 0x00;
            _carState->DisplayMessage.data.Field5 = 0xFF;
            _carState->DisplayMessage.data.Field6 = 0xFF;
            _carState->DisplayMessage.data.Field7 = 0xFF;
            _carState->DisplayMessage.data.Field8 = 0xFF;

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

        currentDoorMessage.MessageType = CAN_POPUP_MSG_DOORS_BOOT_BONNET_REAR_SCREEN_AND_FUEL_TANK_OPEN;
        currentDoorMessage.Category = CAN_POPUP_MSG_SHOW_CATEGORY1;
        currentDoorMessage.DoorStatus1 = 0x00;

        hasPendingPopupMessage = false;
        pendingPopupReadyTime = 0;

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