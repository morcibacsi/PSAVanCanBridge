// VanDisplayHandler.h
#pragma once

#ifndef _VanDisplayHandlerV2_h
    #define _VanDisplayHandlerV2_h

#include "../Handlers/AbstractVanMessageHandler.h"

#include "../../Helpers/VanDataToBridgeToCan.h"
#include "../../Helpers/VanIgnitionDataToBridgeToCan.h"
#include "../../Helpers/DoorStatus.h"
#include "../../Helpers/VanCanDisplayPopupMap.h"

#include "../../Can/Handlers/CanTripInfoHandler.h"
#include "../../Can/Handlers/ICanDisplayPopupHandler.h"
#include "../../Can/Handlers/CanStatusOfFunctionsHandler.h"
#include "../../Can/Handlers/CanWarningLogHandler.h"
#include "../Structs/VanDisplayStructsV2.h"
#include "../../../Config.h"

class VanDisplayHandlerV2 : public AbstractVanMessageHandler {
    ICanDisplayPopupHandler* canPopupHandler;
    CanTripInfoHandler* canTripInfoHandler;
    CanStatusOfFunctionsHandler* canStatusOfFunctionsHandler;
    CanWarningLogHandler* canWarningLogHandler;
    VanCanDisplayPopupMap* popupMapping;

    uint16_t LEFT_STICK_BUTTON_TIME = 5000;

    unsigned long leftStickButtonReturn = 0;
    unsigned long currentTime = 0;

    ~VanDisplayHandlerV2()
    {

    }

public:
    VanDisplayHandlerV2(
        ICanDisplayPopupHandler* _canPopupHandler, 
        CanTripInfoHandler* _canTripInfoHandler, 
        VanCanDisplayPopupMap* _popupMapping,
        CanStatusOfFunctionsHandler* _canStatusOfFunctionsHandler,
        CanWarningLogHandler* _canWarningLogHandler
        )
    {
        canPopupHandler = _canPopupHandler;
        canTripInfoHandler = _canTripInfoHandler;
        popupMapping = _popupMapping;
        canStatusOfFunctionsHandler = _canStatusOfFunctionsHandler;
        canWarningLogHandler = _canWarningLogHandler;
    }

    bool ProcessMessage(
        const uint8_t identByte1,
        const uint8_t identByte2,
        const uint8_t vanMessageWithoutId[],
        const uint8_t messageLength,
        VanDataToBridgeToCan& dataToBridge,
        VanIgnitionDataToBridgeToCan& ignitionDataToBridge,
        DoorStatus& doorStatus) override
    {
        if (!(IsVanIdent(identByte1, identByte2, VAN_ID_DISPLAY_POPUP_V2) && messageLength == 16))
        {
            return false;
        }

        currentTime = millis();

        const VanDisplayPacketV2 packet = DeSerialize<VanDisplayPacketV2>(vanMessageWithoutId);
        if (packet.data.Message != 0xFF)
        {
            CanDisplayPopupItem item;
            item.Category = popupMapping->GetCanCategoryFromVanMessage(packet.data.Message);
            item.MessageType = popupMapping->GetCanMessageIdFromVanMessage(packet.data.Message);
            item.DoorStatus1 = 0;
            item.DoorStatus2 = 0;
            item.KmToDisplay = 0;
            item.IsInited = false;
            item.Counter = 0;
            item.Visible = false;
            item.SetVisibleOnDisplayTime = 0;
            item.VANByte = packet.data.Message;

            switch (packet.data.Message)
            {
                case VAN_POPUP_MSG_FUEL_TANK_ACCESS_OPEN:
                {
                    item.DoorStatus2 = 0x40;
                    break;
                }
                case VAN_POPUP_MSG_TYRES_PUNCTURED:
                {
                    item.DoorStatus1 = 0xFF;
                    break;
                }
                case VAN_POPUP_MSG_SEAT_BELT_REMINDER:
                {
                    item.DoorStatus1 = 0xFF;
                    break;
                }
                case VAN_POPUP_MSG_HILL_HOLDER_ACTIVE:
                {
                    // on some screens it displays seatbelt related message
                    item.DoorStatus1 = 0x08;
                    break;
                }
                case VAN_POPUP_MSG_WHEEL_PRESSURE_SENSOR_BATTERY_LOW:
                case VAN_POPUP_MSG_X_TYRE_PRESSURE_SENSORS_MISSING0:
                case VAN_POPUP_MSG_X_TYRE_PRESSURE_SENSORS_MISSING1:
                case VAN_POPUP_MSG_X_TYRE_PRESSURE_SENSORS_MISSING2:
                {
                    item.DoorStatus1 = 0xFF;
                    break;
                }
                case VAN_POPUP_MSG_HEADLIGHT_BEND_SYSTEM_ACTIVATED:
                {
                    // on some screens it displays seatbelt related message
                    item.DoorStatus1 = 0x02;
                    break;
                }
                default:
                    break;
            }
            if (packet.data.Field5.passenger_airbag_deactivated)
            {
                canStatusOfFunctionsHandler->SetPassengerAirbagDisabled();
            }
            if (packet.data.Field8.automatic_lighting_active)
            {
                canStatusOfFunctionsHandler->SetAutomaticHeadlampEnabled();
            }
            else
            {
                canStatusOfFunctionsHandler->SetAutomaticHeadlampDisabled();
            }
            if (packet.data.Field8.child_safety_activated)
            {
                canStatusOfFunctionsHandler->SetPassengerAirbagDisabled();
            }
            if (packet.data.Field8.deadlocking_active)
            {
                canStatusOfFunctionsHandler->SetAutomaticDoorLockingEnabled();
            }
            if (packet.data.Field2.automatic_gearbox_faulty)
            {
                canWarningLogHandler->SetGearBoxFault();
            }
            if (packet.data.Field4.catalytic_converter_fault || packet.data.Field2.mil)
            {
                canWarningLogHandler->SetEngineFaultRepairNeeded();
            }

            canPopupHandler->QueueNewMessage(item);
        }

        dataToBridge.DashIcons1Field.status.SeatBeltWarning = packet.data.Field5.seatbelt_warning;
        dataToBridge.DashIcons1Field.status.FuelLowLight = packet.data.Field6.fuel_level_low;
        dataToBridge.DashIcons1Field.status.PassengerAirbag = packet.data.Field5.passenger_airbag_deactivated;
        dataToBridge.DashIcons1Field.status.Handbrake = packet.data.Field5.handbrake;
        dataToBridge.DashIcons1Field.status.Abs = packet.data.Field2.abs;
        dataToBridge.DashIcons1Field.status.Esp = packet.data.Field2.esp;
        dataToBridge.DashIcons1Field.status.Mil = packet.data.Field2.mil;
        dataToBridge.DashIcons1Field.status.Airbag = packet.data.Field3.side_airbag_faulty;

        if (packet.data.Field5.seatbelt_warning)
        {
            if (dataToBridge.Speed > 10)
            {
                CanDisplayPopupItem item;
                item.Category = CAN_POPUP_MSG_SHOW_CATEGORY1;
                item.MessageType = CAN_POPUP_MSG_FRONT_SEAT_BELTS_NOT_FASTENED;
                item.DoorStatus1 = CAN_POPUP_SEAT_BELTS_OF_DRIVER;
                item.DoorStatus2 = 0;
                item.KmToDisplay = 0;
                item.IsInited = false;
                item.Counter = 0;
                item.Visible = false;
                item.SetVisibleOnDisplayTime = 0;
                item.VANByte = 0;
                canPopupHandler->QueueNewMessage(item);
            }
            else
            {
                canPopupHandler->ResetSeatBeltWarning();
            }
        }
        else
        {
            canPopupHandler->ResetSeatBeltWarning();
        }

        if (DISPLAY_MODE == 3) LEFT_STICK_BUTTON_TIME = 10;
        if (packet.data.Field6.left_stick_button)
        {
            leftStickButtonReturn = currentTime + LEFT_STICK_BUTTON_TIME;
            ignitionDataToBridge.LeftStickButtonPressed = 1;
            dataToBridge.LeftStickButtonPressed = 1;
        }

        if (currentTime > leftStickButtonReturn)
        {
            ignitionDataToBridge.LeftStickButtonPressed = 0;
            dataToBridge.LeftStickButtonPressed = 0;
        }

        return true;
    }
};

#endif
