#pragma once

#include <cstdint>
#include "IntUnions.h"
#include "CarLedStates.h"
#include "CarIndicatorLights.h"
#include "CarSignalLights.h"
#include "DoorStatus.h"
#include "OdometerStates.h"
#include "CarRadioRemote.h"
#include "CarSteeringWheelRemote.h"
#include "AirConditionerState.h"
#include "DisplayMessage.h"
#include "ParkingAidStatus.h"
#include "CarSettings.h"
#include "AvailableOptions_2010.h"
#include "../Protocol/ProtocolType.hpp"
#include "../BoardConfig.h"

struct CarState
{
    char* Version = (char*)"v3.5.2";
    uint64_t CurrenTime = 0;
    uint64_t LastIgnitionTime = 0;
    uint64_t ReverseDisengagedTime = 0;

    uint8_t Ignition = 0;
    uint8_t EngineRunning = 0;
    //0 - not running, 1 - starting, 2 - running, 3 - stopped
    uint8_t EngineStatus = 0;
    // 0 - stop, 1 - contact, 2 - starter, 3 - free
    uint8_t KeyPosition = 0;
    uint8_t IsReverseEngaged = 0;
    uint8_t IsReverseCameraOn = 0;
    //uint8_t TrailerPresent = 0;
    uint8_t WiperStatus = 0;
    //0 - off, 1 - on, 2 - standby soon
    uint8_t IgnitionMode = 0xFF;
    uint8_t FactoryMode = 0;

    uint8_t ExternalTemperature = 0xFF;
    uint8_t EconomyMode = 0;
    uint8_t NightMode = 0;
    uint8_t BlackPanelStatus = 0;
    uint8_t DashboardBrightness = 0;
    uint8_t ResetMaintenance = 0;
    uint8_t AirbagDisabled = 0;
    uint8_t DistanceUnit = 0;
    uint8_t SpeedInKmh = 0;
    uint8_t AdvisedSpeedFromNacInKmh = 0xFF;
    bool SpeedLimitFromNacAuthorized = false;

    //State helpers
    uint8_t State_AutoLockEnabled = 0xFF;
    uint8_t State_AutoLightsEnabled = 0xFF;
    uint8_t State_AutoWipeEnabled = 0xFF;

    //217 AEE2010
    OdometerStatesStruct OdometerStates{};
    uint8_t SpeedDisplayedOnCmb = 0;

    uint8_t CoolantTemperature = 0;

    //0E1
    ParkingAidStatusStruct ParkingAidStatus{};

    //0E6 AEE2010
    uint8_t ABRFault = 0;
    uint8_t ABSInProgess = 0;
    uint8_t BrakeFluidLevelAlert = 0;
    uint8_t BrakePadsWorn = 0;

    //128 AEE2010
    CarSignalLightsStruct CarSignalLights{};
    uint8_t GearPositionCmb = 0;
    uint8_t GearPositionInDriving = 0;
    uint8_t GearBlinking = 0;
    uint8_t GearBoxMode = 0;
    uint8_t GearBoxSelection = 0;
    uint8_t RecommendedGearBlinking = 0;
    uint8_t RecommendedGearDirection = 0;

    //1A1
    DisplayMessageStruct DisplayMessage{};

    //1E3
    AirConditionerState AirConditionerStatus{};

    //161 AEE2010
    uint8_t OilLevelRestart = 0;
    uint8_t EngineOilTemperature = 0;
    uint8_t FuelLevel = 0;
    uint8_t FuelTankCapacity = 0;
    uint8_t EngineOilLevel = 0;

    //167
    //0 - None
    //2 - Totals
    //4 - Cumulative
    //7 - Trip not managed by EMF
    uint8_t CurrentEmfMode = 7;
    uint8_t ResetTotals = 0;
    uint8_t ResetCumulative = 0;

    //168 AEE2010
    CarIndicatorLightsStruct CarIndicatorLights{};

    //228
    uint8_t CruiseControlSpeedUnit = 0;
    uint8_t CruiseControlActivateFunction = 0;
    uint8_t CruiseControlStatusOfSelectedFunction = 0;
    uint8_t CruiseControlSelectedFunction = 0;

    //0B6
    UInt16 DistanceForCMB = 0;
    UInt16 CruiseControlSpeed = 0;
    UInt16 Speed = 0;
    UInt16 Rpm = 0;
    UInt16 RearLeftWheelCounter = 0;
    UInt16 RearRightWheelCounter = 0;
    UInt24 Odometer = 0;
    UInt24 TripOnCMB = 0;
    uint8_t ConsumptionForCMB = 0;

    uint8_t SpeedValidationInfo = 0;

    //3E7 AEE2010
    uint8_t IsMaintenanceDue = 0;
    uint8_t WrenchIcon = 0;
    uint8_t MaintenanceSignKm = 0;
    uint8_t MaintenanceSignKmBlinking = 0;
    uint8_t MaintenanceSignTime = 0;
    uint8_t MaintenanceSignTimeBlinking = 0;
    UInt16 MaintenanceKilometers = 0;
    UInt16 MaintenanceTime = 0;

    //220
    DoorStatusStruct DoorStatus{};

    //21F
    CarRadioRemoteStruct RadioRemote{};

    //0A2
    CarSteeringWheelRemoteStruct SteeringWheelRemote{};

    //15B, 260, 1DB AEE2004
    CarSettings_Struct CarSettings{};

    //276
    uint8_t Hour = 0;
    uint8_t Minute = 0;
    uint8_t Second = 0;
    uint8_t MDay = 0;
    uint8_t Month = 0;
    uint16_t Year = 0;

    //227 AEE2010
    CarLedStatusByte1Struct CarLedStatusByte1{};
    CarLedStatusByte2Struct CarLedStatusByte2{};
    CarLedStatusByte3Struct CarLedStatusByte3{};
    CarLedStatusByte4Struct CarLedStatusByte4{};

    //221 AEE2010
    uint8_t InvalidConsumption = 0;
    uint8_t InvalidRemainingRange = 0;
    uint8_t LeftStickButtonPushed = 0;
    uint8_t RightStickButtonPushed = 0;

    UInt16 InstantConsumption = 0;
    UInt16 RemainingRange = 0;
    UInt16 TotalRange = 0;

    //2A1
    uint8_t Trip1Speed = 0;
    UInt16 Trip1Distance = 0;
    UInt16 Trip1Consumption = 0;
    UInt16 Trip1LastResetSince = 0;

    //261
    uint8_t Trip2Speed = 0;
    UInt16 Trip2Distance = 0;
    UInt16 Trip2Consumption = 0;
    UInt16 Trip2LastResetSince = 0;

    //2E1
    uint8_t DeadlockActive = 0;
    uint8_t AutoWipingActive = 0;
    uint8_t AutoHeadlampActive = 0;
    uint8_t PassengerAirbag = 0;
    uint8_t EspDeactivated = 0;

    //10B
    UInt16 SteeringAngle = 0;
    uint8_t SteeringSpeed = 0;
    uint8_t SteeringStatus = 0;

    //361 AEE2010
    AvailableOptionsStruct_2010 AvailableOptions{};

    //136 AEE2004
    UInt16 UreaRemaining = 0x3FFF;
    uint8_t ShowUreaRemaining = 0;

    bool DiagConnected = false;

    // config
    bool SAVE_CONFIG = true;
    uint8_t VIN_FOR_HEADUNIT[17];
    uint8_t CONSUMPTION_UNIT = 0;
    uint8_t DISTANCE_UNIT    = 0;
    uint8_t LANGUAGE         = 0b00001;
    uint8_t VOLUME_UNIT      = 0;
    uint8_t TEMPERATURE_UNIT = 0;
    uint8_t AMBIENCE_LEVEL   = 0b110;
    uint8_t SOUND_HARMONY    = 0b00;
    uint8_t GENERATE_POPUP_FOR_DOOR_STATUS = 1;
    uint8_t TIME_FORMAT_24H = 1; // 0 - 12h, 1 - 24h

    uint8_t FUEL_TANK_CAPACITY_IN_LITERS = 60;

    //1 - AEE2001, 2 - AEE2004, 3 - AEE2010
    uint8_t SOURCE_PROTOCOL = 0;
    uint8_t DESTINATION_PROTOCOL = 0;

    bool EMULATE_DISPLAY_ON_DESTINATION = false;
    bool EMULATE_DISPLAY_ON_SOURCE = false;
    bool USE_IGNITION_SIGNAL_FROM_SOURCE_BUS = true;
    bool HAS_RTC = false;
    bool REPLACE_REMOTE_MODE_BTN_WITH_SRC = false;
    bool EMULATE_STEERING_WHEEL_CONTROLS_WITH_STALK = false;
    bool MODIFY_217_WITH_CURRENT_SPEED = false;
    bool SEND_TIME = false;
    bool CONVERT_SPEED_SIGN_FOR_CMB_FROM_NAC = false;

    bool ENABLE_PARKING_AID_SOUND_FROM_SPEAKER = 0;
    bool ENABLE_REVERSE_CAMERA_ON_RTX = false;
    bool SEND_AC_CHANGES_TO_DISPLAY = false;
    // if true then AC changes are sent to the display
    bool SEND_AC_FAN_CHANGES_TO_DISPLAY = false;
    // if true the AC status is queried with a diagnostic message (only works with rev 1.4)
    bool QUERY_AC_STATUS = false;

    //0: none / PSA CAN
    //1: PSA VAN - the distance data from the parking aid is queried with a diagnostic message (only works with rev 1.4)
    //2: Chinese
    uint8_t PARKING_AID_TYPE = 0;

    uint32_t MILEAGE_AT_CMB_TRIP_RESET = 0;
    //the values are in centimeters and are coming from observing the values of a 5008 from 2012
    uint8_t AAS_REAR_CORNER_DISTANCES[4] = { 60, 50, 40, 30 };
    uint8_t AAS_REAR_DISTANCES[4] = { 145, 100, 50, 30 };
    // 0: Aftermarket
    // 1: RD4/RD43
    // 2: RD45
    // 3: RD45 (CAN2010)
    // 4: RD3
    // 5: RTx
    uint8_t RADIO_TYPE = 1;
    uint8_t SPEED_SIGN_SPEED_TOLERANCE_PERCENT = 0;
    uint64_t REVERSE_CAMERA_ON_TIMEOUT_MS = 3000;

#if BOARD_PROTOCOL_TYPES == 1
    #define BOARD_SUPPORTED_SOURCE_PROTOCOLS      { static_cast<uint8_t>(ProtocolType::AEE2001) }
    #define BOARD_SUPPORTED_DESTINATION_PROTOCOLS { static_cast<uint8_t>(ProtocolType::AEE2004), static_cast<uint8_t>(ProtocolType::AEE2010) }
#elif BOARD_PROTOCOL_TYPES == 2
    #define BOARD_SUPPORTED_SOURCE_PROTOCOLS      { static_cast<uint8_t>(ProtocolType::AEE2004) }
    #define BOARD_SUPPORTED_DESTINATION_PROTOCOLS { static_cast<uint8_t>(ProtocolType::AEE2010) }
#elif BOARD_PROTOCOL_TYPES == 3
    #define BOARD_SUPPORTED_SOURCE_PROTOCOLS      { static_cast<uint8_t>(ProtocolType::AEE2001), static_cast<uint8_t>(ProtocolType::AEE2004) }
    #define BOARD_SUPPORTED_DESTINATION_PROTOCOLS { static_cast<uint8_t>(ProtocolType::AEE2004), static_cast<uint8_t>(ProtocolType::AEE2010) }
#endif

    static constexpr uint8_t SOURCE_PROTOCOLS[] = BOARD_SUPPORTED_SOURCE_PROTOCOLS;
    static constexpr uint8_t DESTINATION_PROTOCOLS[] = BOARD_SUPPORTED_DESTINATION_PROTOCOLS;
};
