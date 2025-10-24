#pragma once

#ifndef _MessageHandler_15B_2004_h
    #define _MessageHandler_15B_2004_h

#include <cstdint>

#include "../../../IMessageHandler.hpp"
#include "../../../../Helpers/CarSettings.h"
#include "../../../ImmediateSignal.hpp"

class MessageHandler_15B_2004 : public IMessageHandler<MessageHandler_15B_2004>
{
    private:
        ImmediateSignalCallback _immediateSignalCallback = nullptr;

        BusMessage message
        {
            .id = 0x15B,
            .command = 0,
            .data = {0},
            .dataLength = 8,
            .protocol = ProtocolType::AEE2004,
            .periodicityMs = 1000,
            .offsetMs = 130,
            .isActive = false
        };

        uint8_t GetConfigurableButtonSetting(uint8_t aee2010Value)
        {
            switch (aee2010Value)
            {
                case CONFIGURABLE_BUTTON_2010_CEILING_LIGHT:
                    return CONFIGURABLE_BUTTON_2004_CEILING_LIGHT;
                case CONFIGURABLE_BUTTON_2010_BLACK_PANEL:
                    return CONFIGURABLE_BUTTON_2004_BLACK_PANEL;
                case CONFIGURABLE_BUTTON_2010_ALERT_LOG:
                    return CONFIGURABLE_BUTTON_2004_ALERT_LOG;
                case CONFIGURABLE_BUTTON_2010_CUSTOMIZATION:
                    return CONFIGURABLE_BUTTON_2004_CUSTOMIZATION;
                case CONFIGURABLE_BUTTON_2010_CMB_COLOR:
                    return CONFIGURABLE_BUTTON_2004_CMB_COLOR;
                case CONFIGURABLE_BUTTON_2010_MANUAL_CHECK:
                    return CONFIGURABLE_BUTTON_2004_MANUAL_CHECK;
                default:
                    return CONFIGURABLE_BUTTON_2004_NONE;
            }
        }

        uint8_t GetFollowMeHomeTimeSetting(uint8_t aee2010Value)
        {
            switch (aee2010Value)
            {
                case 0:// 15 seconds
                    return 1;
                case 1:// 30 seconds
                    return 2;
                case 2:// 60 seconds
                    return 4;
                default:
                    return 1;
            }
        }

        uint8_t GetProfileNumber(uint8_t aee2010Value)
        {
            switch (aee2010Value)
            {
                case 1:
                    return 1;
                case 2:
                    return 2;
                default:
                    return 1;
            }
        }
    public:
        static constexpr uint32_t MessageId = 0x15B;

        void SetImmediateSignalCallback(ImmediateSignalCallback immediateSignalCallback) { _immediateSignalCallback = immediateSignalCallback; }

        BusMessage Generate(CarState* carState)
        {
            CarSettings_Byte1Struct field1{};
            field1.asByte = carState->CarSettings.Field1.asByte;
            field1.data.profile_number = GetProfileNumber(carState->CarSettings.ProfileNumber2010);

            CarSettings_Byte3Struct field3{};
            field3.asByte = carState->CarSettings.Field3.asByte;
            field3.data.follow_me_home_time = GetFollowMeHomeTimeSetting(carState->CarSettings.FollowMeHomeTime2010);

            CarSettings_Byte8Struct field8{};
            field8.asByte = carState->CarSettings.Field8.asByte;
            field8.data.configurable_button_function = GetConfigurableButtonSetting(carState->CarSettings.ConfigurableButtonFunction2010);

            message.data[0] = field1.asByte;
            message.data[1] = carState->CarSettings.Field2.asByte;
            message.data[2] = field3.asByte;
            message.data[3] = carState->CarSettings.Field4.asByte;
            message.data[4] = carState->CarSettings.Field5.asByte;
            message.data[5] = carState->CarSettings.Field6.asByte;
            message.data[6] = carState->CarSettings.Field7.asByte;
            message.data[7] = field8.asByte;

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {

        }
};
#endif
