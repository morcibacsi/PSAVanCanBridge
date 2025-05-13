// CanDisplayPopupHandler3.h
#pragma once

#ifndef _CanDisplayPopupHandler3_h
    #define _CanDisplayPopupHandler3_h

#include <memory>

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

    std::shared_ptr<CarState> _carState;

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

    //void ShowDebugMessage(const String msg)
    void ShowDebugMessage(const char* msg)
    {
        //printf(msg);
        //printf("\n");
        //debug_println(msg);
    }

    public:
    CanDisplayPopupHandler3(std::shared_ptr<CarState> carState)
    {
        _carState = std::move(carState);

        currentPopupMessage.MessageType = CAN_POPUP_MSG_NONE;
        currentPopupMessage.Category = CAN_POPUP_MSG_SHOW_CATEGORY3;
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
            ShowDebugMessage("isNonDoorMessage");
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
        ShowDebugMessage("Process");
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
        _carState->DisplayMessage.data.Field8 = byte7;
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

            _carState->DisplayMessage.data.Field1 = CAN_POPUP_MSG_HIDE;
            _carState->DisplayMessage.data.Field2 = currentPopupMessage.MessageType;
            _carState->DisplayMessage.data.Field3 = byte3.asByte;
            _carState->DisplayMessage.data.Field4 = 0x00;
            _carState->DisplayMessage.data.Field5 = 0xFF;
            _carState->DisplayMessage.data.Field6 = 0xFF;
            _carState->DisplayMessage.data.Field8 = 0xFF;
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
