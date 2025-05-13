#pragma once

#ifndef _MessageHandler_524_h
    #define _MessageHandler_524_h

#include <inttypes.h>
#include <memory>
#include <cstring>

#include "../../Structs/VAN_524.h"
#include "../../../AEE2004/Structs/CanDisplayStructs.h"
#include "../../../IMessageHandler.hpp"

#include "CanDisplayPopupItem.h"
#include "CanDisplayPopupHandler3.h"
#include "VanCanDisplayPopupMap.h"

class MessageHandler_524 : public IMessageHandler
{
    std::shared_ptr<CanDisplayPopupHandler3> _canPopupHandler;
    std::shared_ptr<VanCanDisplayPopupMap> _popupMapping;

    uint8_t ChangeAutoSetting(uint8_t messageByte, uint8_t newSetting, uint8_t messageType, uint8_t& settingVar)
    {
        if (messageByte == messageType && (settingVar == 0xFF || settingVar == 0))
        {
            settingVar = newSetting;
            return 1;
        }

        if (settingVar == 0xFF)
        {
            settingVar = newSetting;
            return 0xFF;
        }

        if (settingVar == 1 && newSetting == 0)
        {
            settingVar = newSetting;
            return 0;
        }

        settingVar = newSetting;
        return 0xFF;
    }

    void ParsePopupMessage(std::shared_ptr<CarState> carState, VanDisplayV2Struct vanPacket)
    {
        //TODO implement this
        ///*
        uint8_t message = vanPacket.Message;

        //printf("MessageHandler_524::ParsePopupMessage: %02X\n", message);

        if (message != VAN_POPUP_MSG_NONE && message != VAN_POPUP_MSG_DOOR_OPEN && message != VAN_POPUP_MSG_ESP_ASR_DEACTIVATED)
        {
            CanDisplayPopupItem item{};
            item.Category = _popupMapping->GetCanCategoryFromVanMessage(message);
            item.MessageType = _popupMapping->GetCanMessageIdFromVanMessage(message);
            item.DoorStatus1 = 0;
            item.DoorStatus2 = 0;
            item.KmToDisplay = 0;
            item.IsInited = false;
            item.Counter = 0;
            item.Visible = false;
            item.SetVisibleOnDisplayTime = 0;
            item.VANByte = message;
            switch (message)
            {
                case VAN_POPUP_MSG_AIRBAG_FAULT:
                case VAN_POPUP_MSG_SIDE_AIRBAG_FAULT:
                {
                    item.DoorStatus1 = 0x80;
                    break;
                }
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
                case VAN_POPUP_MSG_DEADLOCKING_ACTIVE:
                {
                    carState->DeadlockActive = 1;
                    break;
                }
                case VAN_POPUP_MSG_AUTOMATIC_LIGHTING_ACTIVE:
                {
                    carState->AutoHeadlampActive = 1;
                    break;
                }
                case VAN_POPUP_MSG_AUTOMATIC_WIPING_ACTIVE:
                {
                    carState->AutoWipingActive = 1;
                    break;
                }
                default:
                    break;
            }

            _canPopupHandler->QueueNewMessage(carState->CurrenTime, item);
        }

        if (message == VAN_POPUP_MSG_NONE)
        {
            if (vanPacket.Field8.esp_asr_deactivated == 0)
            {
                CanDisplayPopupItem item;
                item.Category = CAN_POPUP_MSG_SHOW_CATEGORY2;
                item.MessageType = CAN_POPUP_MSG_ESP_ON_ON_CMB;
                item.DoorStatus1 = 0;
                item.DoorStatus2 = 0;
                item.KmToDisplay = 0;
                item.IsInited = false;
                item.Counter = 0;
                item.Visible = false;
                item.SetVisibleOnDisplayTime = 0;
                item.VANByte = 0;
                _canPopupHandler->QueueNewMessage(carState->CurrenTime, item);
            }
            else
            {
                CanDisplayPopupItem item;
                item.Category = CAN_POPUP_MSG_SHOW_CATEGORY3;
                item.MessageType = CAN_POPUP_MSG_ESP_SYSTEM_DEACTIVATED;
                item.DoorStatus1 = 0;
                item.DoorStatus2 = 0;
                item.KmToDisplay = 0;
                item.IsInited = false;
                item.Counter = 0;
                item.Visible = false;
                item.SetVisibleOnDisplayTime = 0;
                item.VANByte = 0;
                _canPopupHandler->QueueNewMessage(carState->CurrenTime, item);
            }

            if (vanPacket.Field5.seatbelt_warning)
            {
                if (carState->SpeedInKmh > 10)
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
                    _canPopupHandler->QueueNewMessage(carState->CurrenTime, item);
                }
                else
                {
                    _canPopupHandler->ResetSeatBeltWarning(carState->CurrenTime);
                }
            }
            else
            {
                _canPopupHandler->ResetSeatBeltWarning(carState->CurrenTime);
            }
        }

        uint change = ChangeAutoSetting(message, vanPacket.Field8.deadlocking_active, VAN_POPUP_MSG_DEADLOCKING_ACTIVE, carState->State_AutoLockEnabled);
        if (change == 0 || change == 1)
        {
            carState->DeadlockActive = change;
            CanDisplayPopupItem item;
            item.Category = CAN_POPUP_MSG_SHOW_CATEGORY2;
            item.MessageType = (change == 1) ? CAN_POPUP_MSG_AUTOMATIC_DOOR_LOCKING_ACTIVATED : CAN_POPUP_MSG_AUTOMATIC_DOOR_LOCKING_DEACTIVATED;
            item.DoorStatus1 = 0;
            item.DoorStatus2 = 0;
            item.KmToDisplay = 0;
            item.IsInited = false;
            item.Counter = 0;
            item.Visible = false;
            item.SetVisibleOnDisplayTime = 0;
            item.VANByte = 0;
            _canPopupHandler->QueueNewMessage(carState->CurrenTime, item);
        }

        change = ChangeAutoSetting(message, vanPacket.Field8.automatic_wiping_active, VAN_POPUP_MSG_AUTOMATIC_WIPING_ACTIVE, carState->State_AutoWipeEnabled);
        if (change == 0 || change == 1)
        {
            carState->AutoWipingActive = change;
            CanDisplayPopupItem item;
            item.Category = CAN_POPUP_MSG_SHOW_CATEGORY2;
            item.MessageType = (change == 1) ? CAN_POPUP_MSG_AUTOMATIC_SCREEN_WIPE_ACTIVATED : CAN_POPUP_MSG_AUTOMATIC_SCREEN_WIPE_DEACTIVATED;
            item.DoorStatus1 = 0;
            item.DoorStatus2 = 0;
            item.KmToDisplay = 0;
            item.IsInited = false;
            item.Counter = 0;
            item.Visible = false;
            item.SetVisibleOnDisplayTime = 0;
            item.VANByte = 0;
            _canPopupHandler->QueueNewMessage(carState->CurrenTime, item);
        }

        change = ChangeAutoSetting(message, vanPacket.Field8.automatic_lighting_active, VAN_POPUP_MSG_AUTOMATIC_WIPING_ACTIVE, carState->State_AutoLightsEnabled);
        if (change == 0 || change == 1)
        {
            carState->AutoHeadlampActive = change;
            CanDisplayPopupItem item;
            item.Category = CAN_POPUP_MSG_SHOW_CATEGORY2;
            item.MessageType = (change == 1) ? CAN_POPUP_MSG_AUTOMATIC_HEADLAMP_LIGHTING_ACTIVATED : CAN_POPUP_MSG_AUTOMATIC_HEADLAMP_LIGHTING_DEACTIVATED;
            item.DoorStatus1 = 0;
            item.DoorStatus2 = 0;
            item.KmToDisplay = 0;
            item.IsInited = false;
            item.Counter = 0;
            item.Visible = false;
            item.SetVisibleOnDisplayTime = 0;
            item.VANByte = 0;
            _canPopupHandler->QueueNewMessage(carState->CurrenTime, item);
        }
        //*/
    }

    public:
        MessageHandler_524(std::shared_ptr<CanDisplayPopupHandler3> canPopupHandler)
        {
            _canPopupHandler = std::move(canPopupHandler);
            _popupMapping = std::make_shared<VanCanDisplayPopupMap>();
        }

        BusMessage Generate(std::shared_ptr<CarState> state) override
        {
            BusMessage message;
            message.id = 0x524;
            message.periodicityMs = 500;
            message.offsetMs = 20;
            message.protocol = ProtocolType::AEE2001;

            return message;
        }

        void Parse(std::shared_ptr<CarState> carState, const BusMessage& message) override
        {
            constexpr std::size_t ExpectedPacketSize = sizeof(VanDisplayV2Struct);

            uint8_t packetSize = message.dataLength;
            if (packetSize != 10 && packetSize != 14 && packetSize != ExpectedPacketSize)
            {
                return;
            }

            VanDisplayV2Struct vanPacket;
            std::memcpy(&vanPacket, message.data, packetSize);

            carState->CarIndicatorLights.data.break_fluid_alert   = vanPacket.Field0.brake_system_fault;
            carState->CarIndicatorLights.data.coolant_level_alert = vanPacket.Field1.coolant_level_low;
            carState->CarIndicatorLights.data.coolant_temp_max    = vanPacket.Field0.engine_overheating;
            carState->CarIndicatorLights.data.oil_level_alert     = vanPacket.Field1.oil_level_too_low;
            carState->CarIndicatorLights.data.oil_pressure_alert  = vanPacket.Field1.oil_pressure_too_low;
            //TODO there is a difference between AEE2004 and AEE2010
            //carState->CarIndicatorLights.data.number_of_gears     = vanPacket.Field0.engine_oil_temperature_too_high;

            carState->CarIndicatorLights.data.antipollution_fault = vanPacket.Field1.diesel_additive_too_low;
            carState->CarIndicatorLights.data.fap_clogged         = vanPacket.Field1.unblock_diesel_filter;
            carState->CarIndicatorLights.data.tyre_pressure_low   = vanPacket.Field0.tyre_pressure_low;
            carState->CarIndicatorLights.data.tyre_punctured      = vanPacket.Field1.tyre_punctured;

            carState->CarIndicatorLights.data.abs_fault                = vanPacket.Field2.abs;
            carState->CarIndicatorLights.data.brake_pad_fault          = vanPacket.Field2.brake_pads_worn;
            carState->CarIndicatorLights.data.esp_fault                = vanPacket.Field2.esp;
            carState->CarIndicatorLights.data.mil                      = vanPacket.Field2.mil;
            carState->CarIndicatorLights.data.gearbox_fault            = vanPacket.Field2.automatic_gearbox_faulty;
            carState->CarIndicatorLights.data.water_in_diesel          = vanPacket.Field3.water_in_diesel_fuel_filter;
            carState->CarIndicatorLights.data.serious_suspension_fault = vanPacket.Field2.suspension_or_steering_fault;

            carState->CarIndicatorLights.data.antipollution_fault      = vanPacket.Field4.catalytic_converter_fault;
            carState->CarIndicatorLights.data.battery_charge_fault     = vanPacket.Field4.battery_charge_fault;
            //TODO there is a difference between AEE2004 and AEE2010
            //carState->CarIndicatorLights.data.diesel_additive_fault    = vanPacket.Field4.diesel_particle_filter_faulty;

            carState->CarSignalLights.data.fuel_level_low               = vanPacket.Field6.fuel_level_low;
            carState->CarSignalLights.data.driver_seatbelt_warning      = vanPacket.Field5.seatbelt_warning;
            carState->CarSignalLights.data.passenger_airbag_activated   = vanPacket.Field5.passenger_airbag_deactivated;
            carState->CarSignalLights.data.handbrake_signal             = vanPacket.Field5.handbrake;
            //TODO there is a difference between AEE2004 and AEE2010
            //carState->CarSignalLights.data.passenger_airbag_deactivated = vanPacket.Field3.airbags_faulty;

            carState->CarSignalLights.data.esp_inactivated              = vanPacket.Field8.esp_asr_deactivated;
            carState->CarSignalLights.data.stop_light                   = (vanPacket.Field0.engine_overheating ||
                                                                           vanPacket.Field0.engine_oil_temperature_too_high ||
                                                                           vanPacket.Field0.hydraulic_suspension_pressure_faulty ||
                                                                           vanPacket.Field0.automatic_gearbox_temperature_too_high ||
                                                                           vanPacket.Field1.oil_pressure_too_low ||
                                                                           vanPacket.Field1.coolant_level_low ||
                                                                           vanPacket.Field2.braking_system_faulty) ? 1 : 0;

            carState->LeftStickButtonPushed = vanPacket.Field6.left_stick_button;

            ParsePopupMessage(carState, vanPacket);
        }
};
#endif
