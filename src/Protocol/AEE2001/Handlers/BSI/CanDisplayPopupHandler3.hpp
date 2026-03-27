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
    // also need to add that during high load of BSI as for example during deep diagnostic checkup, there was a notice of 300 not being enough. but it still makes most sense

    enum PopupState
    {
        POPUP_STATE_NONE,
        POPUP_STATE_NON_DOOR,
        POPUP_STATE_DOOR,
        POPUP_STATE_TRANSITION
    };

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

    // Explicit pending slot is used only for NON-DOOR messages.
    // Door messages use currentDoorMessage as the latest pending snapshot
    // and are shown after transition when no non-door popup is active/pending.
    CanDisplayPopupItem pendingPopupMessage;
    bool hasPendingPopupMessage = false;
    unsigned long pendingPopupReadyTime = 0;

    PopupState activePopupState = POPUP_STATE_NONE;

    //void ShowDebugMessage(const String msg)
    void ShowDebugMessage(const char* msg)
    {
        //printf(msg);
        //printf("\n");
        //debug_println(msg);
    }

    bool IsDoorMessage(CanDisplayPopupItem message)
    {
        return message.MessageType == CAN_POPUP_MSG_DOORS_BOOT_BONNET_REAR_SCREEN_AND_FUEL_TANK_OPEN;
    }

    bool IsNoPopupMessage(CanDisplayPopupItem message)
    {
        return
            message.Category == CAN_POPUP_MSG_SHOW_CATEGORY3 &&
            message.MessageType == CAN_POPUP_MSG_NONE;
    }

    bool IsSamePopup(CanDisplayPopupItem a, CanDisplayPopupItem b)
    {
        return
            a.Category == b.Category &&
            a.MessageType == b.MessageType;
    }

    void QueuePendingPopup(CanDisplayPopupItem message)
    {
        pendingPopupMessage = message;
        hasPendingPopupMessage = true;
    }

    void ClearPendingPopup()
    {
        hasPendingPopupMessage = false;
    }

    void StartTransition(unsigned long currentTime)
    {
        if (isPopupVisible)
        {
            HideCurrentPopupMessage(currentTime);
        }

        activePopupState = POPUP_STATE_TRANSITION;
        pendingPopupReadyTime = currentTime + POPUP_TRANSITION_PAUSE_TIME;
    }

    void StartNonDoorPopup(unsigned long currentTime, CanDisplayPopupItem message)
    {
        currentPopupMessage = message;
        popupAddedToShow = currentTime;
        activePopupState = POPUP_STATE_NON_DOOR;
        ShowPopupMessage(currentPopupMessage);
    }

    void StartDoorPopup()
    {
        activePopupState = POPUP_STATE_DOOR;
        ShowPopupMessage(currentDoorMessage);
    }

    void FinishTransition(unsigned long currentTime)
    {
        if (hasPendingPopupMessage)
        {
            CanDisplayPopupItem message = pendingPopupMessage;
            ClearPendingPopup();
            StartNonDoorPopup(currentTime, message);
            return;
        }

        if (currentDoorMessage.DoorStatus1 != 0x00)
        {
            StartDoorPopup();
            return;
        }

        activePopupState = POPUP_STATE_NONE;
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
        const bool isIncomingDoorMessage = IsDoorMessage(incomingPopupMessage);
        const bool isNoPopupMessage = IsNoPopupMessage(incomingPopupMessage);

        if (!isIncomingDoorMessage && !isNoPopupMessage)
        {
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
        }

        if (!isNoPopupMessage)
        {
            if ((riskOfIceShown && incomingMessageType == CAN_POPUP_MSG_RISK_OF_ICE) ||
                (seatbeltWarningShown && incomingMessageType == CAN_POPUP_MSG_FRONT_SEAT_BELTS_NOT_FASTENED) ||
                (espActivatedShown && incomingMessageType == CAN_POPUP_MSG_ESP_ON_ON_CMB && !espDeActivatedShown) ||
                (espDeActivatedShown && incomingMessageType == CAN_POPUP_MSG_ESP_SYSTEM_DEACTIVATED && !espActivatedShown)
               )
            {
                return;
            }
        }

        if (isIncomingDoorMessage)
        {
            const uint8_t prevDoorStatus = currentDoorMessage.DoorStatus1;
            currentDoorMessage = incomingPopupMessage;

            // Door popup is low priority fallback only.
            // If it is already visible and the bitmask changed, transition and then show the new state.
            if (activePopupState == POPUP_STATE_DOOR)
            {
                if (incomingPopupMessage.DoorStatus1 != prevDoorStatus)
                {
                    ShowDebugMessage("DoorStateChangedWhileVisible");
                    StartTransition(currentTime);
                }
            }
            else
            {
                // If idle and no pending non-door popup exists, prepare transition to show the door popup.
                if (activePopupState == POPUP_STATE_NONE &&
                    !hasPendingPopupMessage &&
                    incomingPopupMessage.DoorStatus1 != 0x00 &&
                    incomingPopupMessage.DoorStatus1 != prevDoorStatus)
                {
                    ShowDebugMessage("IdleToDoorTransition");
                    StartTransition(currentTime);
                }
            }

            return;
        }

        if (isNoPopupMessage)
        {
            ShowDebugMessage("NoPopupMessageFrom524");

            // If a non-door popup is active, or waiting, 524 explicitly says "no popup now".
            // So clear pending non-door and transition away from current non-door popup.
            ClearPendingPopup();

            if (activePopupState == POPUP_STATE_NON_DOOR)
            {
                StartTransition(currentTime);
                return;
            }

            // If we are already in transition, just let it complete toward fallback popup logic.
            // If idle/door, nothing special needed here.
            return;
        }

        ShowDebugMessage("isNonDoorMessage");

        // If same popup is already visible, refresh its lifetime and payload.
        if (activePopupState == POPUP_STATE_NON_DOOR)
        {
            if (IsSamePopup(incomingPopupMessage, currentPopupMessage))
            {
                currentPopupMessage = incomingPopupMessage;
                popupAddedToShow = currentTime;
                ShowDebugMessage("RefreshCurrentNonDoorPopup");
                return;
            }
        }

        // If same popup is already pending, refresh its payload.
        if (hasPendingPopupMessage)
        {
            if (IsSamePopup(incomingPopupMessage, pendingPopupMessage))
            {
                pendingPopupMessage = incomingPopupMessage;
                ShowDebugMessage("RefreshPendingNonDoorPopup");
                return;
            }
        }

        if (activePopupState == POPUP_STATE_NONE)
        {
            StartNonDoorPopup(currentTime, incomingPopupMessage);
            return;
        }

        // Door popup loses priority immediately to non-door popup.
        if (activePopupState == POPUP_STATE_DOOR)
        {
            QueuePendingPopup(incomingPopupMessage);
            ShowDebugMessage("DoorToNonDoorTransition");
            StartTransition(currentTime);
            return;
        }

        // If already in transition, just keep the newest pending non-door popup.
        if (activePopupState == POPUP_STATE_TRANSITION)
        {
            QueuePendingPopup(incomingPopupMessage);
            ShowDebugMessage("PendingNonDoorUpdatedDuringTransition");
            return;
        }

        // If another non-door popup is visible, the newly asserted 524 popup becomes the next target immediately.
        if (activePopupState == POPUP_STATE_NON_DOOR)
        {
            QueuePendingPopup(incomingPopupMessage);
            ShowDebugMessage("NonDoorToNonDoorTransition");
            StartTransition(currentTime);
            return;
        }
    }

    void Process(unsigned long currentTime)
    {
        if (!isIgnitionOn)
        {
            return;
        }

        ShowDebugMessage("Process");

        if (activePopupState == POPUP_STATE_TRANSITION)
        {
            if (currentTime >= pendingPopupReadyTime)
            {
                ShowDebugMessage("TransitionComplete");
                FinishTransition(currentTime);
            }
            return;
        }

        if (activePopupState == POPUP_STATE_NON_DOOR)
        {
            // This timeout is now only a fallback safety net.
            // The normal lifecycle is driven by repeated 524 popup refreshes or explicit NONE/FF.
            bool shouldHideByTimeOut =
                (currentTime - popupAddedToShow) > CAN_POPUP_MESSAGE_MAX_DISPLAY_TIME
                && (!(currentPopupMessage.Category == CAN_POPUP_MSG_SHOW_CATEGORY3 &&
                      currentPopupMessage.MessageType == CAN_POPUP_MSG_NONE));

            if (shouldHideByTimeOut)
            {
                ShowDebugMessage("HideByTimeoutFallback");
                StartTransition(currentTime);
                return;
            }

            ShowDebugMessage("ShowCurrentPopupMessage");
            ShowPopupMessage(currentPopupMessage);
            return;
        }

        if (activePopupState == POPUP_STATE_DOOR)
        {
            // If a non-door popup arrived while door popup is shown, it has priority.
            if (hasPendingPopupMessage)
            {
                ShowDebugMessage("DoorInterruptedByPendingNonDoor");
                StartTransition(currentTime);
                return;
            }

            // Door popup has no timeout. It stays until the bitmask becomes zero.
            if (currentDoorMessage.DoorStatus1 == 0x00)
            {
                ShowDebugMessage("HideDoorPopupBecauseDoorStatusIsZero");
                StartTransition(currentTime);
                return;
            }

            ShowDebugMessage("ShowCurrentDoorMessage");
            ShowPopupMessage(currentDoorMessage);
            return;
        }

        if (activePopupState == POPUP_STATE_NONE)
        {
            if (hasPendingPopupMessage)
            {
                CanDisplayPopupItem message = pendingPopupMessage;
                ClearPendingPopup();
                StartNonDoorPopup(currentTime, message);
                return;
            }

            if (currentDoorMessage.DoorStatus1 != 0x00)
            {
                ShowDebugMessage("IdleToDoorTransition");
                StartTransition(currentTime);
                return;
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

        if (IsDoorMessage(message))
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
        (void)currentTime;

        if (isPopupVisible)
        {
            CanDisplayByte2Struct byte3{};
            byte3.data.show_popup_on_emf = 0;
            byte3.data.show_popup_on_cmb = 0;
            byte3.data.show_popup_on_vth = 0;
            byte3.data.priority = 1;

            uint8_t popupTypeToHide = currentPopupMessage.MessageType;
            if (activePopupState == POPUP_STATE_DOOR || isDoorMessageVisible)
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

        ClearPendingPopup();
        pendingPopupReadyTime = 0;
        activePopupState = POPUP_STATE_NONE;

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
        (void)currentTime;
        espActivatedShown = true;
        espDeActivatedShown = false;
    }

    void SetEngineRunning(bool isRunning)
    {
        (void)isRunning;
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
            activePopupState == POPUP_STATE_NONE &&
            !hasPendingPopupMessage &&
            currentDoorMessage.DoorStatus1 != 0x00;
    }
};

#endif