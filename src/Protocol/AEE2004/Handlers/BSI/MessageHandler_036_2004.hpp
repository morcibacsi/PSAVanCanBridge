#pragma once

#ifndef _MessageHandler_036_h
    #define _MessageHandler_036_h

#include <cstdint>
#include <cstring>

#include "../../../IMessageHandler.hpp"
#include "../../Structs/CAN_036_2004.h"
#include "../../../FeedbackSignal.hpp"

class MessageHandler_036 : public IMessageHandler<MessageHandler_036>
{
    private:
        BusMessage message
        {
            .id = 0x036,
            .data = { 0 },
            .dataLength = 8,
            .protocol = ProtocolType::AEE2004,
            .periodicityMs = 100,
            .offsetMs = 0,
            .isActive = true
        };

        FeedbackSignalCallback _feedbackSignalCallback;

    public:
        static constexpr uint32_t MessageId = 0x036;

        MessageHandler_036()
        {
        }

        void SetFeedbackSignalCallback(FeedbackSignalCallback feedbackSignalCallback) { _feedbackSignalCallback = feedbackSignalCallback; }

        BusMessage Generate(CarState* carState)
        {
            CanIgnitionByte3Struct ecoField{};
            ecoField.data.economy_mode_active = carState->EconomyMode;

            CanIgnitionByte4Struct brightnessField{};
            brightnessField.data.night_mode           = carState->NightMode;
            brightnessField.data.dashboard_brightness = carState->DashboardBrightness;
            brightnessField.data.black_panel_status   = carState->BlackPanelStatus;

            CanIgnitionByte5Struct ignitionField{};

            CanIgnitionByte7Struct rearCamField{};

            if (carState->RADIO_TYPE == 5 && carState->ENABLE_REVERSE_CAMERA_ON_RTX)
            {
                rearCamField.data.activate_rear_camera = carState->IsReverseEngaged;
            }

            ignitionField.data.ignition_mode = 1;
            if (carState->USE_IGNITION_SIGNAL_FROM_SOURCE_BUS)
            {
                ignitionField.data.ignition_mode = carState->IgnitionMode;
            }



//0x0E, 0x00, 0x00, 0x21, ignitionField.asByte, 0x80, 0x00, 0xA0
/*
            message.data.push_back(0x0E);
            message.data.push_back(0x00);
            message.data.push_back(0x00);
            message.data.push_back(0x21);
            message.data.push_back(ignitionField.asByte);
            message.data.push_back(0x80);
            message.data.push_back(0x00);
            message.data.push_back(0xA0);
*/
///*

            message.data[0] = 0x00;
            message.data[1] = 0x00;
            message.data[2] = ecoField.asByte;
            message.data[3] = brightnessField.asByte;
            message.data[4] = ignitionField.asByte;
            message.data[5] = 0x00;
            message.data[6] = rearCamField.asByte;
            message.data[7] = 0xA0;
//*/
            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {
            CanIgnitionStruct packet;
            std::memcpy(&packet, message.data, sizeof(packet));

            //TODO think about the OdometerStates as they are actually set in the 0x217 message
            carState->EconomyMode         = packet.LoadShedding.data.economy_mode_active;
            carState->DashboardBrightness = packet.Brightness.data.dashboard_brightness;
            carState->NightMode           = packet.Brightness.data.night_mode;
            carState->BlackPanelStatus    = packet.Brightness.data.black_panel_status;
            carState->IgnitionMode        = packet.Ignition.data.ignition_mode;

            if (_feedbackSignalCallback)
            {
                _feedbackSignalCallback(FeedbackSignal::IgnitionChanged);
            }
        }
};
#endif
