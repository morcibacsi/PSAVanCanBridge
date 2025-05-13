#pragma once

#ifndef _MessageHandler_361_h
    #define _MessageHandler_361_h

#include <inttypes.h>
#include <memory>
#include <cstring>

#include "../../Structs/CAN_361_2004.h"
#include "../../../IMessageHandler.hpp"

class MessageHandler_361 : public IMessageHandler
{
    private:
        BusMessage message
        {
            .id = 0x361,
            .data = {0},
            .dataLength = 6,
            .protocol = ProtocolType::AEE2004,
            .periodicityMs = 500,
            .offsetMs = 25,
            .isActive = true
        };

    public:
        BusMessage Generate(std::shared_ptr<CarState> state) override
        {
            Can361_2004Byte2Struct field2{};
            field2.data.permanent_rear_flap_lock = state->PermanentRearFlapLock;
            field2.data.config_enabled           = state->ConfigEnabled;

            Can361_2004Byte3Struct field3{};
            field3.data.auto_lighting                 = state->AutoLighting;
            field3.data.automatic_electric_brake      = state->AutomaticElectricBrake;
            field3.data.follow_me_home                = state->FollowMeHome;
            field3.data.hinge_panel_select            = state->HingePanelSelect;
            field3.data.irc_present                   = state->IrcPresent;

            Can361_2004Byte4Struct field4{};
            field4.data.drl_present        = state->DrlPresent;
            field4.data.rear_wiper_option  = state->RearWiperOption;

            Can361_2004Byte5Struct field5{};
            field5.data.aas_disable              = state->AasDisable;
            field5.data.ambient_lighting         = state->AmbientLighting;
            field5.data.blindspot_monitoring     = state->BlindspotMonitoring;
            field5.data.highway_lighting_present = state->HighwayLightingPresent;

            Can361_2004Byte6Struct field6{};
            field6.data.tnb_present             = state->TnbPresent;
            field6.data.breaking_on_alarm_risk  = state->BreakingOnAlarmRisk;
            field6.data.tpms_present            = state->TpmsPresent;
            field6.data.tpms_reset_present      = state->TpmsResetPresent;

            message.data[0] = 0x00;
            message.data[1] = field2.asByte;
            message.data[2] = field3.asByte;
            message.data[3] = field4.asByte;
            message.data[4] = field5.asByte;
            message.data[5] = field6.asByte;

            return message;
        }

        void Parse(std::shared_ptr<CarState> carState, const BusMessage& message) override
        {
            //Can361_2004Struct tmp;
            //std::memcpy(&tmp, message.data, static_cast<std::size_t>(sizeof(tmp)));
            const auto* tmp = reinterpret_cast<const Can361_2004Struct*>(message.data);

            carState->PermanentRearFlapLock  = tmp->Field2.data.permanent_rear_flap_lock;
            carState->ConfigEnabled          = tmp->Field2.data.config_enabled;

            carState->AutoLighting           = tmp->Field3.data.auto_lighting;
            carState->AutomaticElectricBrake = tmp->Field3.data.automatic_electric_brake;
            carState->FollowMeHome           = tmp->Field3.data.follow_me_home;
            carState->HingePanelSelect       = tmp->Field3.data.hinge_panel_select;
            carState->IrcPresent             = tmp->Field3.data.irc_present;

            carState->DrlPresent             = tmp->Field4.data.drl_present;
            carState->RearWiperOption        = tmp->Field4.data.rear_wiper_option;

            carState->AasDisable             = tmp->Field5.data.aas_disable;
            carState->AmbientLighting        = tmp->Field5.data.ambient_lighting;
            carState->BlindspotMonitoring    = tmp->Field5.data.blindspot_monitoring;
            carState->HighwayLightingPresent = tmp->Field5.data.highway_lighting_present;

            carState->TnbPresent             = tmp->Field6.data.tnb_present;
            carState->BreakingOnAlarmRisk    = tmp->Field6.data.breaking_on_alarm_risk;
            carState->TpmsPresent            = tmp->Field6.data.tpms_present;
            carState->TpmsResetPresent       = tmp->Field6.data.tpms_reset_present;
        }
};
#endif
