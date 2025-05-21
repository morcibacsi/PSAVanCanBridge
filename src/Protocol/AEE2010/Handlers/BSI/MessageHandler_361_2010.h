#pragma once

#ifndef _MessageHandler_361_2010_h
    #define _MessageHandler_361_2010_h

#include <inttypes.h>
#include <memory>
#include "../../../IMessageHandler.hpp"
#include "../../Structs/CAN_361_2010.h"

class MessageHandler_361_2010 : public IMessageHandler<MessageHandler_361_2010>
{
    private:
        BusMessage message
        {
            .id = 0x361,
            .data = {0},
            .dataLength = 7,
            .protocol = ProtocolType::AEE2010,
            .periodicityMs = 500,
            .offsetMs = 180,
            .isActive = true
        };
    public:
        static constexpr uint32_t MessageId = 0x361;

        BusMessage Generate(CarState* carState)
        {
            Can361_2010Byte1Struct field1{};
            field1.data.drl_present              = carState->DrlPresent;
            field1.data.auto_lighting            = carState->AutoLighting;
            field1.data.ambient_lighting         = carState->AmbientLighting;
            field1.data.blindspot_monitoring     = carState->BlindspotMonitoring;
            field1.data.highway_lighting_present = carState->HighwayLightingPresent;
            field1.data.setting_menu_availbale   = 1;

            Can361_2010Byte2Struct field2{};
            field2.data.hinge_panel_select       = carState->HingePanelSelect;
            field2.data.permanent_rear_flap_lock = carState->PermanentRearFlapLock;
            field2.data.follow_me_home           = carState->FollowMeHome;
            field2.data.rear_wiper_option        = carState->RearWiperOption;
            field2.data.aas_disable              = carState->AasDisable;

            Can361_2010Byte3Struct field3{};
            field3.data.automatic_electric_brake = carState->AutomaticElectricBrake;
            field3.data.config_enabled           = carState->ConfigEnabled;
            field3.data.tnb_present              = carState->TnbPresent;

            Can361_2010Byte4Struct field4{};
            field4.data.breaking_on_alarm_risk = carState->BreakingOnAlarmRisk;
            field4.data.irc_present            = carState->IrcPresent;
            field4.data.tpms_present           = carState->TpmsPresent;
            field4.data.tpms_reset_present     = carState->TpmsResetPresent;

            message.data[0] = field1.asByte;
            message.data[1] = field2.asByte;
            message.data[2] = field3.asByte;
            message.data[3] = field4.asByte;
            message.data[4] = 0x00;
            message.data[5] = 0x00;
            message.data[6] = 0x00;

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {

        }
};
#endif
