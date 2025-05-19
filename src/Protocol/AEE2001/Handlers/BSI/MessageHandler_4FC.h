#pragma once

#ifndef _MessageHandler_4FC_h
    #define _MessageHandler_4FC_h

#include <inttypes.h>
#include <memory>
#include <cstring>

#include "../../Structs/VAN_4FC.h"
#include "../../../IMessageHandler.hpp"
#include "VanCanGearboxPositionMap.h"

class MessageHandler_4FC : public IMessageHandler<MessageHandler_4FC>
{
    VanCanGearboxPositionMap* _vanCanGearboxPositionMap;
    ImmediateSignalCallback _immediateSignalCallback;

    public:
        static constexpr uint32_t MessageId = 0x4FC;

        MessageHandler_4FC()
        {
            _vanCanGearboxPositionMap = new VanCanGearboxPositionMap();
        }

        void SetImmediateSignalCallback(ImmediateSignalCallback immediateSignalCallback) { _immediateSignalCallback = immediateSignalCallback; }

        BusMessage Generate(CarState* state)
        {
            BusMessage message;
            message.id = 0x4FC;
            message.periodicityMs = 200;
            message.offsetMs = 20;
            message.protocol = ProtocolType::AEE2001;

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {
            constexpr std::size_t ExpectedPacketSize = sizeof(VanInstrumentClusterV2Structs);

            uint8_t packetSize = message.dataLength;

            if (packetSize != 10 && packetSize != 11 && packetSize != ExpectedPacketSize)
            {
                return;
            }

            VanInstrumentClusterV2Structs vanPacket;
            std::memcpy(&vanPacket, message.data, packetSize);

            uint8_t clusterEnabled = vanPacket.Field1.cluster_enabled;

            //if (!carState->ENABLE_MANUAL_GEARBOX_DISPLAY)
            {
                carState->GearBoxMode = _vanCanGearboxPositionMap->GetGearboxModeFromVanMode(vanPacket.AutomaticGearbox.gearbox_selection_mode);
                carState->GearBoxSelection = _vanCanGearboxPositionMap->GetGearboxSelectionFromVanSelection(vanPacket.AutomaticGearbox.bva_bvmp_selection);

                carState->GearPositionCmb = _vanCanGearboxPositionMap->GetGearboxPositionFromVanPosition(vanPacket.AutomaticGearbox.gear_position);
            }

            carState->OdometerEnabled = clusterEnabled;

            carState->CarSignalLights.data.diesel_pre_heating = vanPacket.Field1.pre_heating_status;
            carState->CarSignalLights.data.esp_in_progress = vanPacket.Field1.esp_in_progress;
            carState->CarSignalLights.data.left_turn_indicator = vanPacket.LightsStatus.left_indicator;
            carState->CarSignalLights.data.right_turn_indicator = vanPacket.LightsStatus.right_indicator;
            carState->CarSignalLights.data.rear_foglight = vanPacket.LightsStatus.rear_fog;
            carState->CarSignalLights.data.front_foglight = vanPacket.LightsStatus.front_fog;
            carState->CarSignalLights.data.low_beam_on = vanPacket.LightsStatus.dipped_beam;
            carState->CarSignalLights.data.high_beam_on = vanPacket.LightsStatus.high_beam;

            carState->EngineOilTemperature = vanPacket.OilTemperature;
            carState->FuelLevel = vanPacket.FuelLevel;
            carState->EngineOilLevel = vanPacket.OilLevel;

            carState->IsMaintenanceDue = vanPacket.MaintenanceKmByte1.data.maintenance_due;
            carState->MaintenanceKilometers.data.leftByte = vanPacket.MaintenanceKmByte1.data.maintenance_km1;
            carState->MaintenanceKilometers.data.rightByte = vanPacket.MaintenanceKmByte2;

            if (_immediateSignalCallback)
            {
                _immediateSignalCallback(ImmediateSignal::CmbSignalLightsChanged);
            }

            if (packetSize == 14)
            {
                //carState->CruiseControlStatus = vanPacket.CruiseControlStatus.asByte;
                //carState->CruiseControlAlert = vanPacket.CruiseControlStatus.data.function_alert;
                carState->CruiseControlSpeedUnit = vanPacket.CruiseControlStatus.data.speed_unit;
                carState->CruiseControlActivateFunction = vanPacket.CruiseControlStatus.data.activate_function;
                carState->CruiseControlStatusOfSelectedFunction = vanPacket.CruiseControlStatus.data.status_of_selected_function;
                carState->CruiseControlSelectedFunction = vanPacket.CruiseControlStatus.data.selected_function;

                carState->CruiseControlSpeed.data.leftByte = vanPacket.CruiseControlSpeedByte1;
                carState->CruiseControlSpeed.data.rightByte = vanPacket.CruiseControlSpeedByte2;

                if (_immediateSignalCallback)
                {
                    _immediateSignalCallback(ImmediateSignal::CruiseControl);
                }
            }
        }
};
#endif
