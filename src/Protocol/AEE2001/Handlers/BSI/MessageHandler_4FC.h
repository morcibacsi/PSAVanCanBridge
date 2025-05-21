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

        BusMessage Generate(CarState* carState)
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

            VanInstrumentClusterV2Structs packet;
            std::memcpy(&packet, message.data, packetSize);

            uint8_t clusterEnabled = packet.Field1.cluster_enabled;

            //
            /*
            if (!carState->ENABLE_MANUAL_GEARBOX_DISPLAY)
            {
                carState->GearBoxMode      = _vanCanGearboxPositionMap->GetGearboxModeFromVanMode(packet.AutomaticGearbox.gearbox_selection_mode);
                carState->GearBoxSelection = _vanCanGearboxPositionMap->GetGearboxSelectionFromVanSelection(packet.AutomaticGearbox.bva_bvmp_selection);

                carState->GearPositionCmb  = _vanCanGearboxPositionMap->GetGearboxPositionFromVanPosition(packet.AutomaticGearbox.gear_position);
            }
            //*/

            carState->OdometerEnabled = clusterEnabled;

            carState->CarSignalLights.data.diesel_pre_heating   = packet.Field1.pre_heating_status;
            carState->CarSignalLights.data.esp_in_progress      = packet.Field1.esp_in_progress;
            carState->CarSignalLights.data.left_turn_indicator  = packet.LightsStatus.left_indicator;
            carState->CarSignalLights.data.right_turn_indicator = packet.LightsStatus.right_indicator;
            carState->CarSignalLights.data.rear_foglight        = packet.LightsStatus.rear_fog;
            carState->CarSignalLights.data.front_foglight       = packet.LightsStatus.front_fog;
            carState->CarSignalLights.data.low_beam_on          = packet.LightsStatus.dipped_beam;
            carState->CarSignalLights.data.high_beam_on         = packet.LightsStatus.high_beam;

            carState->EngineOilTemperature = packet.OilTemperature;
            carState->FuelLevel            = packet.FuelLevel;
            carState->EngineOilLevel       = packet.OilLevel;

            carState->IsMaintenanceDue                     = packet.MaintenanceKmByte1.data.maintenance_due;
            carState->MaintenanceKilometers.data.leftByte  = packet.MaintenanceKmByte1.data.maintenance_km1;
            carState->MaintenanceKilometers.data.rightByte = packet.MaintenanceKmByte2;

            if (_immediateSignalCallback)
            {
                _immediateSignalCallback(ImmediateSignal::CmbSignalLightsChanged);
            }

            if (packetSize == 14)
            {
                //carState->CruiseControlStatus = packet.CruiseControlStatus.asByte;
                //carState->CruiseControlAlert = packet.CruiseControlStatus.data.function_alert;
                carState->CruiseControlSpeedUnit                = packet.CruiseControlStatus.data.speed_unit;
                carState->CruiseControlActivateFunction         = packet.CruiseControlStatus.data.activate_function;
                carState->CruiseControlStatusOfSelectedFunction = packet.CruiseControlStatus.data.status_of_selected_function;
                carState->CruiseControlSelectedFunction         = packet.CruiseControlStatus.data.selected_function;

                carState->CruiseControlSpeed.data.leftByte      = packet.CruiseControlSpeedByte1;
                carState->CruiseControlSpeed.data.rightByte     = packet.CruiseControlSpeedByte2;

                if (_immediateSignalCallback)
                {
                    _immediateSignalCallback(ImmediateSignal::CruiseControl);
                }
            }
        }
};
#endif
