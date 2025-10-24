#pragma once

#ifndef _MessageHandler_361_h
    #define _MessageHandler_361_h

#include <cstdint>
#include <cstring>

#include "../../Structs/CAN_361_2004.h"
#include "../../../IMessageHandler.hpp"

// This contains available options info
class MessageHandler_361 : public IMessageHandler<MessageHandler_361>
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
            .isActive = false
        };

    public:
        static constexpr uint32_t MessageId = 0x361;

        BusMessage Generate(CarState* carState)
        {
            Can361_2004Byte1Struct field1{};
            field1.data.profile_number           = 0;
            field1.data.profil_change_disabled   = carState->AvailableOptions.data.setting_menu_available ? 0 : 1;

            Can361_2004Byte2Struct field2{};
            field2.data.permanent_rear_flap_lock = carState->AvailableOptions.data.permanent_rear_flap_lock;
            field2.data.config_of_key_enabled    = carState->AvailableOptions.data.config_of_key_enabled;

            Can361_2004Byte3Struct field3{};
            field3.data.auto_lighting            = carState->AvailableOptions.data.auto_lighting;
            field3.data.automatic_electric_brake = carState->AvailableOptions.data.automatic_electric_brake;
            field3.data.follow_me_home           = carState->AvailableOptions.data.follow_me_home;
            field3.data.hinge_panel_select       = carState->AvailableOptions.data.hinge_panel_select;
            field3.data.irc_present              = carState->AvailableOptions.data.irc_present;

            Can361_2004Byte4Struct field4{};
            field4.data.drl_present              = carState->AvailableOptions.data.drl_present;
            field4.data.rear_wiper_option        = carState->AvailableOptions.data.rear_wiper_option;

            Can361_2004Byte5Struct field5{};
            field5.data.aas_disable              = carState->AvailableOptions.data.aas_disable;
            field5.data.ambient_lighting         = carState->AvailableOptions.data.ambient_lighting;
            field5.data.blindspot_monitoring     = carState->AvailableOptions.data.blindspot_monitoring;
            field5.data.highway_lighting_present = carState->AvailableOptions.data.highway_lighting_present;

            Can361_2004Byte6Struct field6{};
            field6.data.tnb_present              = carState->AvailableOptions.data.tnb_present;
            field6.data.braking_on_alarm_risk    = carState->AvailableOptions.data.braking_on_alarm_risk;
            field6.data.tpms_present             = carState->AvailableOptions.data.tpms_present;
            field6.data.tpms_reset_present       = carState->AvailableOptions.data.tpms_reset_present;

            message.data[0] = field1.asByte;
            message.data[1] = field2.asByte;
            message.data[2] = field3.asByte;
            message.data[3] = field4.asByte;
            message.data[4] = field5.asByte;
            message.data[5] = field6.asByte;

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {
            Can361_2004Struct packet;
            std::memcpy(&packet, message.data, sizeof(packet));

            carState->AvailableOptions.data.drl_present              = packet.Field4.data.drl_present;
            carState->AvailableOptions.data.auto_lighting            = packet.Field3.data.auto_lighting;
            carState->AvailableOptions.data.ambient_lighting         = packet.Field5.data.ambient_lighting;
            carState->AvailableOptions.data.blindspot_monitoring     = packet.Field5.data.blindspot_monitoring;
            carState->AvailableOptions.data.highway_lighting_present = packet.Field5.data.highway_lighting_present;
            carState->AvailableOptions.data.setting_menu_available   = packet.Field1.data.profil_change_disabled == 0 ? 1 : 0;

            carState->AvailableOptions.data.hinge_panel_select       = packet.Field3.data.hinge_panel_select;
            carState->AvailableOptions.data.permanent_rear_flap_lock = packet.Field2.data.permanent_rear_flap_lock;
            carState->AvailableOptions.data.follow_me_home           = packet.Field3.data.follow_me_home;
            carState->AvailableOptions.data.rear_wiper_option        = packet.Field4.data.rear_wiper_option;
            carState->AvailableOptions.data.aas_disable              = packet.Field5.data.aas_disable;

            carState->AvailableOptions.data.automatic_electric_brake = packet.Field3.data.automatic_electric_brake;
            carState->AvailableOptions.data.config_of_key_enabled    = packet.Field2.data.config_of_key_enabled;
            carState->AvailableOptions.data.tnb_present              = packet.Field6.data.tnb_present;

            carState->AvailableOptions.data.braking_on_alarm_risk    = packet.Field6.data.braking_on_alarm_risk;
            carState->AvailableOptions.data.irc_present              = packet.Field3.data.irc_present;
            carState->AvailableOptions.data.tpms_present             = packet.Field6.data.tpms_present;
            carState->AvailableOptions.data.tpms_reset_present       = packet.Field6.data.tpms_reset_present;
        }
};
#endif
