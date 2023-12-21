#pragma once

#ifndef _DataBroker_h
    #define _DataBroker_h

#include <stdint.h>
#include "IntUnions.h"

class DataBroker
{
    public:
        uint8_t Ignition;
        uint8_t StandBySoon;
        uint8_t EngineRunning;
        uint8_t IsReverseEngaged;
        uint8_t TrailerPresent;
        uint8_t WiperStatus;
        uint8_t IsTripPressed;
        uint8_t Brightness;
        uint8_t BlackPanel;
        uint8_t EconomyMode;
        uint8_t NightMode;
        uint8_t FuelLevel;
        uint8_t GearBoxMode;
        uint8_t GearBoxSelection;
        uint8_t GearPosition;
        uint8_t OilTemperature;
        uint8_t OilLevel;
        uint8_t SideLights;
        uint8_t PreHeatingStatus;
        uint8_t EspInRegulation;
        uint8_t LeftTurnIndicator;
        uint8_t RightTurnIndicator;
        uint8_t RearFog;
        uint8_t FrontFog;
        uint8_t LowBeam;
        uint8_t HighBeam;
        uint8_t FuelLow;
        uint8_t SeatbeltWarning;
        uint8_t PassengerAirbag;
        uint8_t Handbrake;
        uint8_t Airbag;
        uint8_t EspDeactivated;

        uint8_t CruiseControlStatus;
        uint8_t CruiseControlAlert;
        uint8_t CruiseControlSpeedUnit;
        uint8_t CruiseControlActivateFunction;
        uint8_t CruiseControlStatusOfSelectedFunction;
        uint8_t CruiseControlSelectedFunction;

        uint8_t OdometerEnabled;
        uint8_t DeadlockActive;
        uint8_t AutoWipingActive;
        uint8_t AutoHeadlampActive;

        uint8_t RearLeftWheelCounter1;
        uint8_t RearLeftWheelCounter2;
        uint8_t RearRightWheelCounter1;
        uint8_t RearRightWheelCounter2;
        uint8_t SteeringAngleByte1;
        uint8_t SteeringAngleByte2;
        uint8_t SteeringSpeedByte;
        uint8_t SteeringStatusByte;
        uint8_t Consumption;
        uint8_t DistanceForCMB1;
        uint8_t DistanceForCMB2;

        uint8_t AutoGearboxAlert;
        uint8_t BreakFluidAlert;
        uint8_t CoolantLevelAlert;
        uint8_t CoolantTempMax;
        uint8_t OilLevelAlert;
        uint8_t OilPressureAlert;
        uint8_t OilTempMax;

        uint8_t DieselAdditiveAlert;
        uint8_t FapClogged;
        uint8_t FlatTyreAlert;
        uint8_t TyrePressureAlert;

        uint8_t AbsFault;
        uint8_t BrakePadFault;
        uint8_t EspFault;
        uint8_t Mil;
        uint8_t AutoGearboxFault;
        uint8_t WaterInDiesel;
        uint8_t SeriousSuspensionFault;
        uint8_t StopOnCMB;

        uint8_t AntipollutionFault;
        uint8_t BatteryChargeFault;
        uint8_t DieselAdditiveFault;

        /*trip*/
        uint8_t RemainingRangeByte1;
        uint8_t RemainingRangeByte2;
        uint8_t InstantConsumptionByte1;
        uint8_t InstantConsumptionByte2;
        uint8_t TotalRangeByte1;
        uint8_t TotalRangeByte2;

        uint8_t Trip1Speed;
        uint8_t Trip1DistanceByte1;
        uint8_t Trip1DistanceByte2;
        uint8_t Trip1ConsumptionByte1;
        uint8_t Trip1ConsumptionByte2;

        uint8_t DistanceByte1;
        uint8_t DistanceByte2;
        uint8_t ConsumptionByte1;
        uint8_t ConsumptionByte2;
        uint8_t AverageSpeed;
        uint8_t LeftStickButtonPushed;
        /*trip*/
        uint8_t IsHeatingPanelOn;
        uint8_t IsRecyclingOn;
        uint8_t IsAirConEnabled;
        uint8_t IsACCompressorOn;
        uint8_t FanSpeed;
        uint8_t IsWindowHeatingOn;

        uint8_t ParkingExteriorRearLeft  = 0xFF;
        uint8_t ParkingExteriorRearRight = 0xFF;
        uint8_t ParkingInteriorRearLeft  = 0xFF;
        uint8_t ParkingInteriorRearRight = 0xFF;
        uint8_t ParkingSoundEnabled;
        uint8_t HasParkingRadarData = 0;

        uint8_t IsVanLogEnabled = 1;
        uint8_t PageDisplayedOnTypeC = 7; //ODOMETER_TRIP_INFO_NOT_MANAGED_BY_EMF from CAN message with id 0x167 - this way when there is no type C is installed we reset everything
        uint16_t ProcessedPackets;

        bool IsActivated;
        uint8_t ExternalTemperature;
        uint8_t CoolantTemperature;
        uint8_t RadioRemoteScroll;
        uint8_t RadioRemoteButton = 0x00;

        uint8_t FuelFlapOpen;
        uint8_t RearWindowOpen;
        uint8_t HoodOpen;
        uint8_t TrunkOpen;
        uint8_t RearRightDoorOpen;
        uint8_t RearLeftDoorOpen;
        uint8_t FrontRightDoorOpen;
        uint8_t FrontLeftDoorOpen;

        UInt24 Odometer;
        UInt24 TripOnCMB;
        uint8_t IsMaintenanceDue;
        UInt16 MaintenanceKilometers;
        UInt16 CruiseControlSpeed;
        UInt16 Speed;
        UInt16 Rpm;
        uint8_t SpeedInKmh; //speed in km/h

        uint8_t IsMenuOpenOnCanEMF;
};

#endif
