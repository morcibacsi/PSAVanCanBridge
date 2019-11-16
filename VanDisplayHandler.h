// VanDisplayHandler.h
#pragma once

#ifndef _VanDisplayHandler_h
    #define _VanDisplayHandler_h

#include "AbstractVanMessageHandler.h"

#include "VanDataToBridgeToCan.h"
#include "VanIgnitionDataToBridgeToCan.h"
#include "DoorStatus.h"

#include "CanTripInfoHandler.h"
#include "CanDisplayPopupHandler.h"
#include "VanDisplayStructs.h"

class VanDisplayHandler : public AbstractVanMessageHandler {
    CanDisplayPopupHandler* canPopupHandler;
    CanTripInfoHandler* canTripInfoHandler;
    CanStatusOfFunctionsHandler* canStatusOfFunctionsHandler;
    CanWarningLogHandler* canWarningLogHandler;
    VanCanDisplayPopupMap* popupMapping;

    ~VanDisplayHandler()
    {

    }

public:
    VanDisplayHandler(
        CanDisplayPopupHandler* _canPopupHandler, 
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
        if (!(IsVanIdent(identByte1, identByte2, VAN_ID_DISPLAY_POPUP) && messageLength == 14))
        {
            return false;
        }

        const VanDisplayPacket packet = DeSerialize<VanDisplayPacket>(vanMessageWithoutId);
        if (packet.data.Message != 0xFF)
        {
            CanDisplayPopupItem item;
            item.DisplayTimeInMilliSeconds = CAN_POPUP_MESSAGE_TIME;
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
                //case VAN_POPUP_MSG_DEADLOCKING_ACTIVE:
                //    canStatusOfFunctionsHandler->SetAutomaticDoorLockingEnabled();
                //    break;
                //case VAN_POPUP_MSG_AUTOMATIC_LIGHTING_ACTIVE:
                //    canStatusOfFunctionsHandler->SetAutomaticHeadlampEnabled();
                //    break;
                //case VAN_POPUP_MSG_AUTOMATIC_LIGHTING_INACTIVE:
                //    canStatusOfFunctionsHandler->SetAutomaticHeadlampDisabled();
                //    break;
                //case VAN_POPUP_MSG_PASSENGER_AIRBAG_DEACTIVATED:
                //    canStatusOfFunctionsHandler->SetPassengerAirbagDisabled();
                //    break;
                //case VAN_POPUP_MSG_CATALYTIC_CONVERTER_FAULT:
                //case VAN_POPUP_MSG_ANTIPOLLUTION_FAULT:
                //    canWarningLogHandler->SetEngineFaultRepairNeeded();
                //    break;
                //case VAN_POPUP_MSG_AUTOMATIC_GEAR_FAULT:
                //    canWarningLogHandler->SetGearBoxFault();
                //    break;
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
                item.DisplayTimeInMilliSeconds = CAN_POPUP_MESSAGE_TIME;
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
        if (packet.data.Field6.left_stick_button)
        {
            canTripInfoHandler->TripButtonPress();
        }

        return true;
    }
};

#endif
