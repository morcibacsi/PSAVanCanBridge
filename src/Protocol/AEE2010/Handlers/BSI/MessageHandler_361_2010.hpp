#pragma once

#ifndef _MessageHandler_361_2010_h
    #define _MessageHandler_361_2010_h

#include <cstdint>

#include "../../../IMessageHandler.hpp"
#include "../../Structs/CAN_361_2010.h"

// This contains available options info
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
            field1.data.drl_present              = carState->AvailableOptions.data.drl_present;
            field1.data.auto_lighting            = carState->AvailableOptions.data.auto_lighting;
            field1.data.ambient_lighting         = carState->AvailableOptions.data.ambient_lighting;
            field1.data.blindspot_monitoring     = carState->AvailableOptions.data.blindspot_monitoring;
            field1.data.highway_lighting_present = carState->AvailableOptions.data.highway_lighting_present;
            field1.data.setting_menu_available   = carState->AvailableOptions.data.setting_menu_available;

            Can361_2010Byte2Struct field2{};
            field2.data.hinge_panel_select       = carState->AvailableOptions.data.hinge_panel_select;
            field2.data.permanent_rear_flap_lock = carState->AvailableOptions.data.permanent_rear_flap_lock;
            field2.data.follow_me_home           = carState->AvailableOptions.data.follow_me_home;
            field2.data.rear_wiper_option        = carState->AvailableOptions.data.rear_wiper_option;
            field2.data.aas_disable              = carState->AvailableOptions.data.aas_disable;

            Can361_2010Byte3Struct field3{};
            field3.data.automatic_electric_brake = carState->AvailableOptions.data.automatic_electric_brake;
            field3.data.config_of_key_enabled    = carState->AvailableOptions.data.config_of_key_enabled;
            field3.data.tnb_present              = carState->AvailableOptions.data.tnb_present;

            Can361_2010Byte4Struct field4{};
            field4.data.braking_on_alarm_risk  = carState->AvailableOptions.data.braking_on_alarm_risk;
            field4.data.irc_present            = carState->AvailableOptions.data.irc_present;
            field4.data.tpms_present           = carState->AvailableOptions.data.tpms_present;
            field4.data.tpms_reset_present     = carState->AvailableOptions.data.tpms_reset_present;

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
