#include "CanDataSenderTask.h"
#include <string.h>

#include "../Can/Handlers/MessageHandler_1A1.h"

#include "../Van/Structs/VAN_8A4.h"
#include "../Van/Structs/VAN_4FC.h"
#include "../Van/Structs/VAN_524.h"
#include "../Van/Structs/VAN_564.h"
#include "../Van/Structs/VAN_464.h"
#include "../Van/Structs/VAN_4DC.h"
#include "../Van/Structs/VAN_744.h"
#include "../Van/Structs/VAN_554.h"
#include "../Van/Structs/VAN_4D4.h"
#include "../Van/Structs/VAN_8C4.h"
#include "../Van/Structs/VAN_8D4.h"
#include "../Van/Structs/VanDisplayPopupMessage.h"

#include "../Can/Structs/CAN_128.h"
#include "../Can/Structs/CAN_10B.h"
#include "../Helpers/CanDisplayPopupItem.h"
#include "../Helpers/DebugPrint.h"

CanDataSenderTask::CanDataSenderTask(
                        ICanMessageSender *canMessageSender,
                        Config *config,
                        DataBroker *dataBroker,
                        AbsSer *serial,
                        CanMessageHandlerContainer *canMessageHandlerContainer,
                        VanWriterTask *vanWriterTask
                      )

{
    _canMessageHandlerContainer = canMessageHandlerContainer;

    _canMessageSender = canMessageSender;
    _dataBroker = dataBroker;
    _serial = serial;

    _config = config;

    _vanWriterTask = vanWriterTask;

    _vanSoundOptionState = new VanSoundOptionState();
    _vanRadioState = new VanRadioState(_config, _vanWriterTask, _vanSoundOptionState, _canMessageHandlerContainer);
    _gearCalculator = new GearCalculator(_config, _dataBroker);
    _carState = new CarState();
    _displayRemote = new DisplayRemote(_canMessageHandlerContainer, _dataBroker, _config);

    _vanCanGearboxPositionMap = new VanCanGearboxPositionMap();
    _popupMapping = new VanCanDisplayPopupMap();
    _vanCanAirConditionerSpeedMap = new VanCanAirConditionerSpeedMap();

    _mileage.asUint24 = 0;

    MessageHandler_1A1* messageHandler_1A1 = static_cast<MessageHandler_1A1*>(_canMessageHandlerContainer->GetHandler(0x1A1));
    _canPopupHandler = new CanDisplayPopupHandler3(canMessageSender, serial, messageHandler_1A1);

    if (!_config->USE_IGNITION_SIGNAL_FROM_VAN_BUS)
    {
        _canPopupHandler->SetIgnition(0, 1);
        _dataBroker->Ignition = 1;
        _dataBroker->EngineRunning = 1;

        _canMessageHandlerContainer->SetData(0x036);
        _canMessageHandlerContainer->SetData(0x0F6);
    }
    _canMessageHandlerContainer->SetData(0x260);
}

void IRAM_ATTR CanDataSenderTask::ProcessVanMessage(unsigned long currentTime, uint8_t vanMessage[], uint8_t msgLength)
{
    _currentTime = currentTime;

    //unsigned long stopwatch = millis();
    uint8_t identByte1 = vanMessage[1];
    uint8_t identByte2 = vanMessage[2];
    vanMessageLengthWithoutId = msgLength - 5;

    //make a copy of the buffer excluding the ids and the crc (otherwise deserializing the packet gives wrong results)
    memcpy(vanData, vanMessage + 3, vanMessageLengthWithoutId);

    uint16_t combined = identByte1 << 8 | identByte2;
    uint16_t vanId = combined >> 4;

    uint8_t ignition = 1;
    uint8_t engineRunning = 1;

    switch (vanId)
    {
        case 0x8A4:
            HandleIgnition(ignition, engineRunning);
            break;
        case 0x4FC:
            if (_config->BRIDGE_ODOMETER_DATA)
            {
                HandleOdometer();
            }
            break;
        case 0x744:
            HandleRearWheelData();
            break;
        case 0x9C4:
            HandleRadioRemote();
            break;
        case 0x824:
            HandleSpeedRpm();
            break;
        case 0x524:
            HandleAlerts();
            break;
        case 0x564:
            HandleTripData();
            //send ignition data to avoid timeout because of the time lost in this if branch

            _canMessageHandlerContainer->SendMessageForced(0x036, _currentTime);
            break;
        case 0x464:
            if (_config->SEND_AC_CHANGES_TO_DISPLAY)
            {
                HandleAC1_464();
            }
            break;
        case 0x4DC:
            if (_config->SEND_AC_CHANGES_TO_DISPLAY)
            {
                HandleAC2_4DC();
            }
            break;
        case 0xA68:
            HandleParkingRadarGetDistance();
            break;
        case 0xAE8:
            HandleParkingRadar();
            break;
        case 0x4D4:
            if (_config->BRIDGE_RADIO_DATA)
            {
                HandleRadioSettings();
            }
            break;
        case 0x554:
            if (_config->BRIDGE_RADIO_DATA)
            {
                HandleRadio();
            }
            break;
        case 0x8C4:
            HandleEvent();
            break;
        case 0x8D4:
            HandleRadioCommand();
            break;
        case 0xE24:
            HandleVin();
            break;
        default:
            break;
    }
    //_serial->println(millis() - stopwatch);
}

void CanDataSenderTask::SendCanMessage(unsigned long currentTime)
{
    if (_config->USE_IGNITION_SIGNAL_FROM_VAN_BUS)
    {
        if (currentTime - _prevIgnitionHandledTime > 600)
        {
            //ShutOffIgnition();
        }
    }

    _vanRadioState->Process(currentTime);

    //if (_scheduler->IsMessageDue(currentTime, 0x1A1))
    {
        _canPopupHandler->Process(currentTime);
        //_messageHandler_1A1->SendMessage();
    }
}

void IRAM_ATTR CanDataSenderTask::HandleIgnition(uint8_t ignition, uint8_t engineRunning)
{
    if (vanMessageLengthWithoutId != 7)
    {
        return;
    }

    VanDashboardStructs tmp;
    memcpy(&tmp, vanData, sizeof(tmp));

    if (_config->USE_IGNITION_SIGNAL_FROM_VAN_BUS)
    {
        ignition = tmp.Field1.ignition_on || tmp.Field1.accesories_on || tmp.Field1.engine_running;
        engineRunning = tmp.Field1.engine_running;
    }

    _dataBroker->Ignition = ignition;

    if (!ignition)
    {
        _carState->Reset();
    }

    _dataBroker->StandBySoon = tmp.Field1.standby_mode;
    _dataBroker->EngineRunning = engineRunning;
    _dataBroker->IsReverseEngaged = tmp.Field1.reverse_gear;
    _dataBroker->ExternalTemperature = tmp.ExternalTemperature;
    _dataBroker->CoolantTemperature = tmp.CoolantTemperature;
    if (_dataBroker->IsReverseEngaged == 0)
    {
        _dataBroker->HasParkingRadarData = 0;
    }

    SendManualGear();

    _canPopupHandler->SetIgnition(_currentTime, ignition);

     //external temperature is between -3 and +3 degrees celcius (inclusive)
    if (_dataBroker->Ignition && _currentTime > 10000 && _dataBroker->ExternalTemperature >= 0x4A && _dataBroker->ExternalTemperature <= 0x56)
    {
        //show "Ice alert"
        if (!_canPopupHandler->IsPopupVisible())
        {
            CanDisplayPopupItem item;
            item.Category = CAN_POPUP_MSG_SHOW_CATEGORY1;
            item.MessageType = CAN_POPUP_MSG_RISK_OF_ICE;
            item.DoorStatus1 = 0;
            item.DoorStatus2 = 0;
            item.IsInited = false;
            item.Counter = 0;
            item.Visible = false;
            item.SetVisibleOnDisplayTime = 0;
            item.VANByte = 0;
            _canPopupHandler->QueueNewMessage(_currentTime, item);
        }
    }

    _mileage.data.leftByte = tmp.MileageByte1;
    _mileage.data.middleByte = tmp.MileageByte2;
    _mileage.data.rightByte = tmp.MileageByte3;

    _dataBroker->Odometer.asUint24 = _mileage.asUint24;

    _canMessageHandlerContainer->SetData(0x0F6);
    _canMessageHandlerContainer->SetData(0x1E3);
    _canMessageHandlerContainer->SetData(0x126);
    _canMessageHandlerContainer->SetData(0x0E1);

    UInt24 tripOnCMB;
    tripOnCMB.asUint24 = (_mileage.asUint24 - _config->MILEAGE_AT_CMB_TRIP_RESET)*100;

    //max: 1e847fh = 1999.999 -> 1999.9
    //     1e841bh = 1999.899 -> 1999.8


    if (tripOnCMB.asUint24 > 0x1e847f)
    {
        _dataBroker->TripOnCMB.asUint24 = 0x1e847f;
    }
    else
    {
        _dataBroker->TripOnCMB.asUint24 = tripOnCMB.asUint24;
    }

    _canMessageHandlerContainer->SetData(0x1A8);

    // when we don't use the brigthness settings from the CAN odometer, then we do what the original V2C did: simply send half value as the brightness
    if (!_config->USE_BRIGHTNESS_FROM_CAN_ODOMETER)
    {
        _dataBroker->Brightness =  tmp.Field0.is_backlight_off == 1 ? 15 : 7;
        _dataBroker->BlackPanel = tmp.Field0.black_panel_status;
    }

    /*
    _serial->print("backlight: ");_serial->println(tmp.Field0.is_backlight_off);
    _serial->print("bright: ");_serial->println(_brightness);
    _serial->print("black: ");_serial->println(_blackPanel);
    */

   _dataBroker->EconomyMode = tmp.Field1.economy_mode;
   _dataBroker->NightMode = tmp.Field0.is_backlight_off == 1 ? 0 : 1;
   _dataBroker->SideLights = _dataBroker->NightMode;

   _canMessageHandlerContainer->SetData(0x036);
   _canMessageHandlerContainer->SetData(0x0E1);
   _canMessageHandlerContainer->SetData(0x128);
}

void IRAM_ATTR CanDataSenderTask::HandleOdometer()
{
    if ((vanMessageLengthWithoutId == 10 || vanMessageLengthWithoutId == 11 || vanMessageLengthWithoutId == 14))
    {
        uint8_t clusterEnabled = 1;
        VanInstrumentClusterV2Structs vanPacket;
        memcpy(&vanPacket, vanData, sizeof(vanPacket));

        if (_config->USE_IGNITION_SIGNAL_FROM_VAN_BUS)
        {
            clusterEnabled = vanPacket.Field1.cluster_enabled;
        }
        //clusterEnabled = 0;

        if (!_config->ENABLE_MANUAL_GEARBOX_DISPLAY)
        {
            _dataBroker->GearBoxMode = _vanCanGearboxPositionMap->GetGearboxModeFromVanMode(vanPacket.AutomaticGearbox.gearbox_selection_mode);
            _dataBroker->GearBoxSelection = _vanCanGearboxPositionMap->GetGearboxSelectionFromVanSelection(vanPacket.AutomaticGearbox.bva_bvmp_selection);

            _dataBroker->GearPosition = _vanCanGearboxPositionMap->GetGearboxPositionFromVanPosition(vanPacket.AutomaticGearbox.gear_position);
        }

        _dataBroker->OdometerEnabled = clusterEnabled;

        _dataBroker->PreHeatingStatus = vanPacket.Field1.pre_heating_status;
        _dataBroker->EspInRegulation = vanPacket.Field1.esp_in_progress;
        _dataBroker->LeftTurnIndicator = vanPacket.LightsStatus.left_indicator;
        _dataBroker->RightTurnIndicator = vanPacket.LightsStatus.right_indicator;
        _dataBroker->RearFog = vanPacket.LightsStatus.rear_fog;
        _dataBroker->FrontFog = vanPacket.LightsStatus.front_fog;
        _dataBroker->LowBeam = vanPacket.LightsStatus.dipped_beam;
        _dataBroker->HighBeam = vanPacket.LightsStatus.high_beam;

        _canMessageHandlerContainer->SetData(0x128);

        _dataBroker->OilTemperature = vanPacket.OilTemperature;
        _dataBroker->FuelLevel = vanPacket.FuelLevel;
        _dataBroker->OilLevel = vanPacket.OilLevel;
        _canMessageHandlerContainer->SetData(0x161);

        //maintenance
        _dataBroker->IsMaintenanceDue = vanPacket.MaintenanceKmByte1.data.maintenance_due;
        _dataBroker->MaintenanceKilometers.data.leftByte = vanPacket.MaintenanceKmByte1.data.maintenance_km1;
        _dataBroker->MaintenanceKilometers.data.rightByte = vanPacket.MaintenanceKmByte2;
        _canMessageHandlerContainer->SetData(0x3A7);
        _canMessageHandlerContainer->SetData(0x3E7);

        //cruise control
        if (vanMessageLengthWithoutId == 14)
        {
            _dataBroker->CruiseControlStatus = vanPacket.CruiseControlStatus.asByte;

            _dataBroker->CruiseControlAlert = vanPacket.CruiseControlStatus.data.function_alert;
            _dataBroker->CruiseControlSpeedUnit = vanPacket.CruiseControlStatus.data.speed_unit;
            _dataBroker->CruiseControlActivateFunction = vanPacket.CruiseControlStatus.data.activate_function;
            _dataBroker->CruiseControlStatusOfSelectedFunction = vanPacket.CruiseControlStatus.data.status_of_selected_function;
            _dataBroker->CruiseControlSelectedFunction = vanPacket.CruiseControlStatus.data.selected_function;

            _dataBroker->CruiseControlSpeed.data.leftByte = vanPacket.CruiseControlSpeedByte1;
            _dataBroker->CruiseControlSpeed.data.rightByte = vanPacket.CruiseControlSpeedByte2;

            _canMessageHandlerContainer->SetData(0x1A8);
            _canMessageHandlerContainer->SetData(0x228);
            _canMessageHandlerContainer->SendMessageForced(0x1A8, _currentTime);
            _canMessageHandlerContainer->SendMessageForced(0x228, _currentTime);
        }

        _canMessageHandlerContainer->SendMessageForced(0x128, _currentTime);
        _canMessageHandlerContainer->SendMessageForced(0x161, _currentTime);
    }
}

void IRAM_ATTR CanDataSenderTask::HandleRearWheelData()
{
    if (vanMessageLengthWithoutId != 12)
    {
        return;
    }

    VanRearWheelDataStructs vanPacket;
    memcpy(&vanPacket, vanData, sizeof(vanPacket));

    CAN_10B_Byte4Struct canStatus;
    canStatus.asByte = vanPacket.SteeringStatus.asByte;
    canStatus.data.rotation_direction = vanPacket.SteeringStatus.data.rotation_direction == 0 ? 1 : 0;

    _dataBroker->RearLeftWheelCounter1 = vanPacket.RearLeftWheelCounter1;
    _dataBroker->RearLeftWheelCounter2 = vanPacket.RearLeftWheelCounter2;
    _dataBroker->RearRightWheelCounter1 = vanPacket.RearRightWheelCounter1;
    _dataBroker->RearRightWheelCounter2 = vanPacket.RearRightWheelCounter2;

    _dataBroker->SteeringAngleByte1 = vanPacket.SteeringAngle1;
    _dataBroker->SteeringAngleByte2 = vanPacket.SteeringAngle2;
    _dataBroker->SteeringSpeedByte = vanPacket.SteeringSpeed;
    _dataBroker->SteeringStatusByte = canStatus.asByte;

    _canMessageHandlerContainer->SetData(0x0E6);
    _canMessageHandlerContainer->SetData(0x10B);
}

void IRAM_ATTR CanDataSenderTask::HandleRadioRemote()
{
    if (vanMessageLengthWithoutId != 2)
    {
        return;
    }
    _dataBroker->RadioRemoteButton = vanData[0];
    _dataBroker->RadioRemoteScroll = vanData[1];
    _canMessageHandlerContainer->SetData(0x21F);
    _canMessageHandlerContainer->SendMessageForced(0x21F, _currentTime);
}

void IRAM_ATTR CanDataSenderTask::HandleSpeedRpm()
{
    if (vanMessageLengthWithoutId != 7)
    {
        return;
    }

    uint16_t vanSpeed = ((uint16_t)vanData[2] << 8) | vanData[3];

    if (vanSpeed == 0xFFFF)
    {
        _dataBroker->SpeedInKmh = 0;
    }
    else
    {
        _dataBroker->SpeedInKmh = vanSpeed/100;
    }

    _dataBroker->Rpm.data.leftByte = vanData[0];
    _dataBroker->Rpm.data.rightByte = vanData[1];

    _dataBroker->Speed.data.leftByte = vanData[2];
    _dataBroker->Speed.data.rightByte = vanData[3];

    _dataBroker->DistanceForCMB1 = vanData[4];
    _dataBroker->DistanceForCMB2 = vanData[5];

    _dataBroker->Consumption = vanData[6];

    _canMessageHandlerContainer->SetData(0x0B6);
    _canMessageHandlerContainer->SendMessageForced(0x0B6, _currentTime);

    _canMessageHandlerContainer->SetData(0x297);
    _canMessageHandlerContainer->SendMessageForced(0x297, _currentTime);
    SendManualGear();
}

void IRAM_ATTR CanDataSenderTask::HandleAlerts()
{
    ///*
    if (!(vanMessageLengthWithoutId == 10 || vanMessageLengthWithoutId == 14 || vanMessageLengthWithoutId == 16))
    {
        return;
    }
    //*/
    VanDisplayV2Struct vanPacket;
    memcpy(&vanPacket, vanData, sizeof(vanPacket));

    {
        _dataBroker->BreakFluidAlert        = vanPacket.Field0.brake_system_fault;
        _dataBroker->CoolantLevelAlert      = vanPacket.Field1.coolant_level_low;
        _dataBroker->CoolantTempMax         = vanPacket.Field0.engine_overheating;
        _dataBroker->OilLevelAlert          = vanPacket.Field1.oil_level_too_low;
        _dataBroker->OilPressureAlert       = vanPacket.Field1.oil_pressure_too_low;
        _dataBroker->OilTempMax             = vanPacket.Field0.engine_oil_temperature_too_high;

        _dataBroker->DieselAdditiveAlert    = vanPacket.Field1.diesel_additive_too_low;
        _dataBroker->FapClogged             = vanPacket.Field1.unblock_diesel_filter;
        _dataBroker->FlatTyreAlert          = vanPacket.Field0.tyre_pressure_low;
        _dataBroker->TyrePressureAlert      = vanPacket.Field1.tyres_punctured;

        _dataBroker->AbsFault               = vanPacket.Field2.abs;
        _dataBroker->BrakePadFault          = vanPacket.Field2.brake_pads_worn;
        _dataBroker->EspFault               = vanPacket.Field2.esp;
        _dataBroker->Mil                    = vanPacket.Field2.mil;
        _dataBroker->AutoGearboxFault       = vanPacket.Field2.automatic_gearbox_faulty;
        _dataBroker->WaterInDiesel          = vanPacket.Field3.water_in_diesel_fuel_filter;
        _dataBroker->SeriousSuspensionFault = vanPacket.Field2.suspension_or_steering_fault;

        _dataBroker->AntipollutionFault     = vanPacket.Field4.catalytic_converter_fault;
        _dataBroker->BatteryChargeFault     = vanPacket.Field4.battery_charge_fault;
        _dataBroker->DieselAdditiveFault    = vanPacket.Field4.diesel_particle_filter_faulty;

        _dataBroker->FuelLow                = vanPacket.Field6.fuel_level_low;
        _dataBroker->SeatbeltWarning        = vanPacket.Field5.seatbelt_warning;
        _dataBroker->PassengerAirbag        = vanPacket.Field5.passenger_airbag_deactivated;
        _dataBroker->Handbrake              = vanPacket.Field5.handbrake;
        _dataBroker->Airbag                 = vanPacket.Field3.airbags_faulty;
        _dataBroker->EspDeactivated         = vanPacket.Field8.esp_asr_deactivated;

        _dataBroker->StopOnCMB              = (vanPacket.Field0.engine_overheating ||
                                              vanPacket.Field0.engine_oil_temperature_too_high ||
                                              vanPacket.Field0.hydraulic_suspension_pressure_faulty ||
                                              vanPacket.Field0.automatic_gearbox_temperature_too_high ||
                                              vanPacket.Field1.oil_pressure_too_low ||
                                              vanPacket.Field1.coolant_level_low ||
                                              vanPacket.Field2.braking_system_faulty) ? 1 : 0;

        _canMessageHandlerContainer->SetData(0x128);
        _canMessageHandlerContainer->SetData(0x168);
    }

    _dataBroker->LeftStickButtonPushed = vanPacket.Field6.left_stick_button;
    _canMessageHandlerContainer->SetData(0x221);
    _canMessageHandlerContainer->SendMessageForced(0x221, _currentTime);
    _canMessageHandlerContainer->SetData(0x120);

    uint8_t message = vanData[9];

    if (message != VAN_POPUP_MSG_NONE && message != VAN_POPUP_MSG_DOOR_OPEN && message != VAN_POPUP_MSG_ESP_ASR_DEACTIVATED)
    {
        CanDisplayPopupItem item;
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
                _dataBroker->DeadlockActive = 1;
                break;
            }
            case VAN_POPUP_MSG_AUTOMATIC_LIGHTING_ACTIVE:
            {
                _dataBroker->AutoHeadlampActive = 1;
                break;
            }
            case VAN_POPUP_MSG_AUTOMATIC_WIPING_ACTIVE:
            {
                _dataBroker->AutoWipingActive = 1;
                break;
            }
            default:
                break;
        }

        _canPopupHandler->QueueNewMessage(_currentTime, item);
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
            _canPopupHandler->QueueNewMessage(_currentTime, item);
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
            _canPopupHandler->QueueNewMessage(_currentTime, item);
        }

        if (vanPacket.Field5.seatbelt_warning)
        {
            if (_dataBroker->SpeedInKmh > 10)
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
                _canPopupHandler->QueueNewMessage(_currentTime, item);
            }
            else
            {
                _canPopupHandler->ResetSeatBeltWarning(_currentTime);
            }
        }
        else
        {
            _canPopupHandler->ResetSeatBeltWarning(_currentTime);
        }
    }

    uint change = _carState->ChangeAutoLock(message, vanPacket.Field8.deadlocking_active);
    if (change == 0 || change == 1)
    {
        _dataBroker->DeadlockActive = change;
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
        _canPopupHandler->QueueNewMessage(_currentTime, item);

    }

    change = _carState->ChangeAutoWipe(message, vanPacket.Field8.automatic_wiping_active);
    if (change == 0 || change == 1)
    {
        _dataBroker->AutoWipingActive = change;
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
        _canPopupHandler->QueueNewMessage(_currentTime, item);
    }

    change = _carState->ChangeAutoLights(message, vanPacket.Field8.automatic_lighting_active);
    if (change == 0 || change == 1)
    {
        _dataBroker->AutoHeadlampActive = change;
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
        _canPopupHandler->QueueNewMessage(_currentTime, item);
    }
    _canMessageHandlerContainer->SetData(0x2E1);
    _canPopupHandler->Process(_currentTime);
}

void IRAM_ATTR CanDataSenderTask::HandleTripData()
{
    if (vanMessageLengthWithoutId != 27)
    {
        return;
    }

    VanCarStatusWithTripComputerStruct vanPacket;
    memcpy(&vanPacket, vanData, sizeof(vanPacket));

    _dataBroker->FrontLeftDoorOpen  = vanPacket.Doors.FrontLeft;
    _dataBroker->FrontRightDoorOpen = vanPacket.Doors.FrontRight;
    _dataBroker->RearLeftDoorOpen   = vanPacket.Doors.RearLeft;
    _dataBroker->RearRightDoorOpen  = vanPacket.Doors.RearRight;
    _dataBroker->TrunkOpen          = vanPacket.Doors.BootLid;
    _dataBroker->HoodOpen           = vanPacket.Doors.Hood;
    _dataBroker->FuelFlapOpen       = vanPacket.Doors.FuelFlap;
    _dataBroker->RearWindowOpen     = vanPacket.Doors.Sunroof;
    _canMessageHandlerContainer->SetData(0x220);

    CanDisplayDoorStatus1Struct doorStatus;
    doorStatus.asByte = 0;
    doorStatus.data.is_front_left_open  = vanPacket.Doors.FrontLeft;
    doorStatus.data.is_front_right_open = vanPacket.Doors.FrontRight;
    doorStatus.data.is_rear_left_open   = vanPacket.Doors.RearLeft;
    doorStatus.data.is_rear_right_open  = vanPacket.Doors.RearRight;
    doorStatus.data.is_boot_open        = vanPacket.Doors.BootLid;
    doorStatus.data.is_bonnet_open      = vanPacket.Doors.Hood;

    CanDisplayDoorStatus2Struct doorStatus2;
    doorStatus2.asByte = 0;
    doorStatus2.data.is_fuel_flap_open = vanPacket.Doors.FuelFlap;

    //if (doorStatus.asByte > 0 || doorStatus2.asByte > 0)
    {
        //
        /*
        char tmp[3];
        for (uint8_t i = 0; i < 20; i++)
        {
            snprintf(tmp, 3, "%02X", vanData[i]);
            if (i != vanMessageLengthWithoutId - 1)
            {
                _serial->print(tmp);
                _serial->print(" ");
            }
            else
            {
                _serial->println(tmp);
            }
        }

        _serial->println(doorStatus.asByte, HEX);
        _serial->println(doorStatus2.asByte, HEX);
        //*/
        CanDisplayPopupItem item;
        item.Category = CAN_POPUP_MSG_SHOW_CATEGORY1;
        item.MessageType = CAN_POPUP_MSG_DOORS_BOOT_BONNET_REAR_SCREEN_AND_FUEL_TANK_OPEN;
        item.DoorStatus1 = doorStatus.asByte;
        item.DoorStatus2 = doorStatus2.asByte;
        item.KmToDisplay = 0;
        item.IsInited = false;
        item.Counter = 0;
        item.Visible = false;
        item.SetVisibleOnDisplayTime = 0;
        item.VANByte = 0x02;
        _canPopupHandler->QueueNewMessage(_currentTime, item);
    }

    // -------- Page auto icon1 --------
    uint8_t iconA1byte1 = vanPacket.FuelLeftToPumpInKm.data.rightByte;
    uint8_t iconA1byte2 = vanPacket.FuelLeftToPumpInKm.data.leftByte;

    // -------- Page auto icon3 --------
    uint8_t iconA3byte1 = vanPacket.Trip1Distance.data.rightByte;
    uint8_t iconA3byte2 = vanPacket.Trip1Distance.data.leftByte;

    if (_config->DISPLAY_MODE == 2)
    {
        UInt16 twoByteData1;
        twoByteData1.asUint16 = round(_config->FUEL_TANK_CAPACITY_IN_LITERS * _dataBroker->FuelLevel / 100);
        iconA1byte1 = twoByteData1.data.leftByte;
        iconA1byte2 = twoByteData1.data.rightByte;

        UInt16 twoByteData2;
        twoByteData2.asUint16 = _dataBroker->SpeedInKmh*10;
        iconA3byte1 = twoByteData2.data.leftByte;
        iconA3byte2 = twoByteData2.data.rightByte;
    }

    // -------- Page auto --------
    uint8_t fuelConsumptionByte1 = vanPacket.FuelConsumption.data.rightByte;
    uint8_t fuelConsumptionByte2 = vanPacket.FuelConsumption.data.leftByte;

    _dataBroker->IsTripPressed = vanPacket.Field10.TripButton;
    _dataBroker->RemainingRangeByte1 = iconA1byte1;
    _dataBroker->RemainingRangeByte2 = iconA1byte2;
    _dataBroker->InstantConsumptionByte1 = fuelConsumptionByte1;
    _dataBroker->InstantConsumptionByte2 = fuelConsumptionByte2;
    _dataBroker->TotalRangeByte1 = iconA3byte1;
    _dataBroker->TotalRangeByte2 = iconA3byte2;

    _canMessageHandlerContainer->SetData(0x221);
    _canMessageHandlerContainer->SendMessageForced(0x221, _currentTime);

    // -------- Page auto --------

    // -------- Page 1 --------
    fuelConsumptionByte1 = vanPacket.Trip1FuelConsumption.data.rightByte;
    fuelConsumptionByte2 = vanPacket.Trip1FuelConsumption.data.leftByte;

    _dataBroker->Trip1Speed = vanPacket.AvgCourseSpeed;
    _dataBroker->Trip1DistanceByte1 = vanPacket.Trip1Distance.data.rightByte;
    _dataBroker->Trip1DistanceByte2 = vanPacket.Trip1Distance.data.leftByte;
    _dataBroker->Trip1ConsumptionByte1 = fuelConsumptionByte1;
    _dataBroker->Trip1ConsumptionByte2 = fuelConsumptionByte2;

    if (vanPacket.Trip1FuelConsumption.asUint16 == 0xFFFF) {
        fuelConsumptionByte1 = 0;
        fuelConsumptionByte2 = 0;
    }

    _canMessageHandlerContainer->SetData(0x2A1);
    _canMessageHandlerContainer->SendMessageForced(0x2A1, _currentTime);

    // -------- Page 1 --------


    // -------- Page 2 --------
    fuelConsumptionByte1 = vanPacket.Trip2FuelConsumption.data.rightByte;
    fuelConsumptionByte2 = vanPacket.Trip2FuelConsumption.data.leftByte;

    if (vanPacket.Trip2FuelConsumption.asUint16 == 0xFFFF) {
        fuelConsumptionByte1 = 0;
        fuelConsumptionByte2 = 0;
    }

    _dataBroker->AverageSpeed = vanPacket.AvgTravelSpeed;
    _dataBroker->DistanceByte1 = vanPacket.Trip2Distance.data.rightByte;
    _dataBroker->DistanceByte2 = vanPacket.Trip2Distance.data.leftByte;
    _dataBroker->ConsumptionByte1 = fuelConsumptionByte1;
    _dataBroker->ConsumptionByte2 = fuelConsumptionByte2;

    _canMessageHandlerContainer->SetData(0x261);
    _canMessageHandlerContainer->SendMessageForced(0x261, _currentTime);
    // -------- Page 2 --------
}

void IRAM_ATTR CanDataSenderTask::HandleAC1_464()
{
    if (vanMessageLengthWithoutId != 5)
    {
        return;
    }

    VanAirConditioner1Struct vanPacket;
    memcpy(&vanPacket, vanData, sizeof(vanPacket));

    uint8_t isRecyclingOn = vanPacket.Status.data.recycling_on;
    uint8_t isAirConEnabled = 0;
    uint8_t airConFanSpeed = 0;
    uint8_t isHeatingPanelOn = 1;

    if (vanData[0] == 0x00 && (vanPacket.PowerConsumption == 0x00))
    {
        isHeatingPanelOn = 0;
    }
    if (
           (vanData[0] == 0x00 && (vanPacket.PowerConsumption == 0x00))  // off
        || (vanData[0] == 0x00 && (vanPacket.PowerConsumption == 0x0E))  // off + rear window heating
        || (vanData[0] == 0x01 && (vanPacket.PowerConsumption == 0x0E))  // off + rear window heating toggle
        || (vanData[0] == 0x04 && (vanPacket.PowerConsumption == 0x00))  // off + recycle
        || (vanData[0] == 0x04 && (vanPacket.PowerConsumption == 0x0E))  // off + rear window heating + recycle
        || (vanData[0] == 0x05 && (vanPacket.PowerConsumption == 0x00))  // off + rear window heating + recycle toggle
        || (vanData[0] == 0x05 && (vanPacket.PowerConsumption == 0x0E))  // off + rear window heating + recycle toggle
        )
    {
        //nothing to do
    }
    else
    {
        isAirConEnabled = vanPacket.Status.data.aircon_requested;
        airConFanSpeed = _vanCanAirConditionerSpeedMap->GetFanSpeedFromVANByte(vanPacket.PowerConsumption, _dataBroker->IsACCompressorOn, _dataBroker->IsWindowHeatingOn, isRecyclingOn);
    }

    _dataBroker->IsHeatingPanelOn = isHeatingPanelOn;
    _dataBroker->IsRecyclingOn = isRecyclingOn;
    _dataBroker->IsAirConEnabled = isAirConEnabled;
    _dataBroker->FanSpeed = airConFanSpeed;

    _canMessageHandlerContainer->SetData(0x1E3);
    _canMessageHandlerContainer->SetData(0x1D0);
    _canMessageHandlerContainer->SetData(0x350);
}

void IRAM_ATTR CanDataSenderTask::HandleAC2_4DC()
{
    if (vanMessageLengthWithoutId != 7)
    {
        return;
    }

    VanAirConditioner2Struct vanPacket;
    memcpy(&vanPacket, vanData, sizeof(vanPacket));

    _dataBroker->IsWindowHeatingOn = vanPacket.Status1.rear_window_heating_on;
    _dataBroker->IsACCompressorOn = vanPacket.Status1.ac_compressor_auth_on;

    _canMessageHandlerContainer->SetData(0x1E3);
}

void IRAM_ATTR CanDataSenderTask::HandleParkingRadarGetDistance()
{
    if (vanMessageLengthWithoutId != 2)
    {
        return;
    }

    if (vanData[0] == 0x21 && vanData[1] == 0xA0)
    {
        _vanWriterTask->QueryParkingRadarData(_currentTime);
    }
}

void IRAM_ATTR CanDataSenderTask::HandleParkingRadar()
{
    if (vanMessageLengthWithoutId != 24 || _config->PARKING_AID_TYPE != 1)
    {
        return;
    }

    if (vanData[2] == 0xA0)
    {
        _dataBroker->HasParkingRadarData = 1;
        uint8_t enableSoundFromSpeaker = 0;
        if (_config->ENABLE_PARKING_AID_SOUND_FROM_SPEAKER)
        {
            enableSoundFromSpeaker = 1;
        }
        _dataBroker->ParkingSoundEnabled      = enableSoundFromSpeaker;
        _dataBroker->ParkingExteriorRearLeft  = vanData[3];
        _dataBroker->ParkingExteriorRearRight = vanData[4];
        _dataBroker->ParkingInteriorRearLeft  = vanData[5];
        _dataBroker->ParkingInteriorRearRight = vanData[6];

        _canMessageHandlerContainer->SetData(0x0E1);
    }
}

void IRAM_ATTR CanDataSenderTask::HandleRadio()
{
    {
        // Tuner
        if (vanMessageLengthWithoutId == 22 && vanData[1] == VAN_RADIO_SOURCE_FREQ_INFO)
        {
            VanRadioTunerStruct vanPacket;
            memcpy(&vanPacket, vanData, sizeof(vanPacket));

            _vanRadioState->Tuner(_currentTime, vanPacket);
        }
        // CD
        if (vanMessageLengthWithoutId == 19 && vanData[1] == VAN_RADIO_SOURCE_CD_INFO)
        {
            VanRadioCDStruct cdVanPacket;
            memcpy(&cdVanPacket, vanData, sizeof(cdVanPacket));

            _vanRadioState->CD(_currentTime, cdVanPacket);
        }
        // Tape
        if (vanMessageLengthWithoutId == 5 && vanData[1] == VAN_RADIO_SOURCE_TAPE_DATA)
        {
            VanRadioTapeStruct tapeVanPacket;
            memcpy(&tapeVanPacket, vanData, sizeof(tapeVanPacket));

            _vanRadioState->Tape(_currentTime, tapeVanPacket);
        }
    }
}

void IRAM_ATTR CanDataSenderTask::HandleRadioSettings()
{
    {
        if (vanMessageLengthWithoutId != 11
            ///*
            ||
            (prevRadioSettings[0] == vanData[1] &&
            prevRadioSettings[1] == vanData[2] &&
            prevRadioSettings[2] == vanData[3] &&
            prevRadioSettings[3] == vanData[4] &&
            prevRadioSettings[4] == vanData[5] &&
            prevRadioSettings[5] == vanData[6] &&
            prevRadioSettings[6] == vanData[7] &&
            prevRadioSettings[7] == vanData[8] &&
            prevRadioSettings[8] == vanData[9]
            )
            //*/
            )
        {
            return;
        }

        prevRadioSettings[0] = vanData[1];
        prevRadioSettings[1] = vanData[2];
        prevRadioSettings[2] = vanData[3];
        prevRadioSettings[3] = vanData[4];
        prevRadioSettings[4] = vanData[5];
        prevRadioSettings[5] = vanData[6];
        prevRadioSettings[6] = vanData[7];
        prevRadioSettings[7] = vanData[8];
        prevRadioSettings[8] = vanData[9];

        VanRadioInfoStruct vanPacket;
        memcpy(&vanPacket, vanData, sizeof(vanPacket));

        _vanRadioState->RadioSettings(_currentTime, vanPacket);
    }
}

void IRAM_ATTR CanDataSenderTask::HandleRadioCommand()
{
    {
        #ifdef PRINT_DEBUG
        char strBuf[50];
        sprintf(strBuf, "Id: %02X", vanData[0]);debug_println(strBuf);
        #endif

        if (vanData[0] == VAN_RADIO_COMMAND_GEN_CMD_ID_GEN) //0x11
        {
            VAN_RAD_CMD_GEN_Struct vanGenPacket;
            memcpy(&vanGenPacket, vanData, sizeof(vanGenPacket));

            #ifdef PRINT_DEBUG
            sprintf(strBuf, "vanGenPacket.Status.data.power_on: %d", vanGenPacket.Status.data.power_on);debug_println(strBuf);
            sprintf(strBuf, "vanGenPacket.Status.data.active_on: %d", vanGenPacket.Status.data.active_on);debug_println(strBuf);
            sprintf(strBuf, "vanGenPacket.Status.data.auto_volume_on: %d", vanGenPacket.Status.data.auto_volume_on);debug_println(strBuf);
            sprintf(strBuf, "vanGenPacket.Status.data.button_mode: %d", vanGenPacket.Status.data.button_mode);debug_println(strBuf);
            sprintf(strBuf, "vanGenPacket.Status.data.generate_beep: %d", vanGenPacket.Status.data.generate_beep);debug_println(strBuf);
            sprintf(strBuf, "vanGenPacket.Status.data.loudness_on: %d", vanGenPacket.Status.data.loudness_on);debug_println(strBuf);
            sprintf(strBuf, "vanGenPacket.Status.data.mute: %d", vanGenPacket.Status.data.mute);debug_println(strBuf);
            #endif
        }

        if (vanData[0] == VAN_RADIO_COMMAND_GEN_CMD_ID_SRC) //0x12
        {
            VAN_RAD_CMD_SRC_Struct vanGenPacket;
            memcpy(&vanGenPacket, vanData, sizeof(vanGenPacket));

            #ifdef PRINT_DEBUG
            sprintf(strBuf, "vanGenPacket.Source.data.source: %d", vanGenPacket.Source.data.source);debug_println(strBuf);
            #endif
        }
    }
}

void IRAM_ATTR CanDataSenderTask::HandleEvent()
{
    VanEventByte1Struct eventSource;
    eventSource.asByte = vanData[0];

    //_serial->printf("SRC: ");
    //_serial->println(eventSource.data.event_src, HEX);

    {
        if (eventSource.data.event_src == VAN_EVENT_SRC_RADIO)
        {
            VanEventRadioStructs vanPacket;
            memcpy(&vanPacket, vanData, sizeof(vanPacket));

            _vanRadioState->RadioEvent(_currentTime, vanPacket);
        }
    }

    if (eventSource.data.event_src == VAN_EVENT_SRC_BSI)
    {
        VanEventBSIStructs vanPacket;
        memcpy(&vanPacket, vanData, sizeof(vanPacket));

        if (vanPacket.Event.data.trip_change_status == 1 || vanPacket.Event.data.door_change_status == 1)
        {
            _vanWriterTask->GetCarStatus(_currentTime);
        }
    }
}

void IRAM_ATTR CanDataSenderTask::HandleVin()
{
}

void CanDataSenderTask::SetBrightness(uint8_t brightness, uint8_t blackPanel)
{
    _dataBroker->Brightness = brightness;
    _dataBroker->BlackPanel = blackPanel;

    _canMessageHandlerContainer->SetData(0x036);
}

void CanDataSenderTask::ResetTripOnCMB()
{
    _config->MILEAGE_AT_CMB_TRIP_RESET = _mileage.asUint24;

    UInt24 tripOnCMB;
    tripOnCMB.asUint24 = (_mileage.asUint24 - _config->MILEAGE_AT_CMB_TRIP_RESET)*100;

    _dataBroker->TripOnCMB.asUint24 = tripOnCMB.asUint24;
    _canMessageHandlerContainer->SetData(0x1A8);
}

void CanDataSenderTask::ShutOffIgnition()
{
    _canPopupHandler->SetIgnition(_currentTime, 0);
    _dataBroker->Ignition = 0;
    _dataBroker->EngineRunning = 0;
    _dataBroker->OdometerEnabled = 0;

    _canMessageHandlerContainer->SetData(0x036);
    _canMessageHandlerContainer->SetData(0x0F6);
    _canMessageHandlerContainer->SetData(0x1E3);
    _canMessageHandlerContainer->SetData(0x128);
}

void CanDataSenderTask::IgnitionPacketArrived(unsigned long currentTime)
{
    _prevIgnitionHandledTime = currentTime;
}

void CanDataSenderTask::Process(unsigned long currentTime)
{
    _displayRemote->Process(currentTime);
}

void CanDataSenderTask::SendManualGear()
{
    if (_config->ENABLE_MANUAL_GEARBOX_DISPLAY)
    {
        _dataBroker->GearBoxMode = CAN_DASH_GEAR_MODE_AUTO;
        _dataBroker->GearBoxSelection = CAN_DASH_GEAR_SELECTION_BVA;
        _dataBroker->GearPosition = _gearCalculator->GetCurrentGear();

        _canMessageHandlerContainer->SetData(0x128);
        _canMessageHandlerContainer->SendMessageForced(0x128, _currentTime);
    }
}