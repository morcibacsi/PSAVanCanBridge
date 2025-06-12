#pragma once

#ifndef _MessageHandler_8A4_h
    #define _MessageHandler_8A4_h

#include <cstdint>
#include <cstring>

#include "../../Structs/VAN_8A4.h"
#include "../../../IMessageHandler.hpp"
#include "CanDisplayPopupHandler3.hpp"
#include "../../../FeedbackSignal.hpp"
#include "../../../ImmediateSignal.hpp"

class MessageHandler_8A4 : public IMessageHandler<MessageHandler_8A4>
{
    CanDisplayPopupHandler3* _canPopupHandler;
    ImmediateSignalCallback _immediateSignalCallback;
    FeedbackSignalCallback _feedbackSignalCallback;

    public:
        static constexpr uint32_t MessageId = 0x8A4;

        MessageHandler_8A4()
        {
        }

        void SetImmediateSignalCallback(ImmediateSignalCallback immediateSignalCallback) { _immediateSignalCallback = immediateSignalCallback; }
        void SetFeedbackSignalCallback(FeedbackSignalCallback feedbackSignalCallback) { _feedbackSignalCallback = feedbackSignalCallback; }
        void SetCanDisplayPopupHandler(CanDisplayPopupHandler3* canPopupHandler) { _canPopupHandler = canPopupHandler; }

        BusMessage Generate(CarState* carState)
        {
            BusMessage message{};
            message.id = MessageId;
            message.periodicityMs = 500;
            message.offsetMs = 20;
            message.protocol = ProtocolType::AEE2001;
            message.type = MessageType::Normal;
            message.ack = false;
            message.dataLength = sizeof(VanDashboardStructs);
            message.isActive = false;

            VanDashboardByte0Struct field0{};
            field0.data.brightness         = 15;

            VanDashboardByte1Struct field1{};
            field1.data.key_position    = 3;
            field1.data.engine_running  = 1;
            field1.data.standby_mode    = 0;
            field1.data.economy_mode    = 0;
            field1.data.reverse_gear    = 1;
            field1.data.factory_mode    = 0;
            field1.data.alarm_active    = 0;

            message.data[0] = field0.asByte;
            message.data[1] = field1.asByte;
            message.data[2] = 0xFF;
            message.data[3] = 0x0F;
            message.data[4] = 0x0F;
            message.data[5] = 0xAA;
            message.data[6] = 0x74;

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {
            constexpr std::size_t ExpectedPacketSize = sizeof(VanDashboardStructs);

            if (message.dataLength != ExpectedPacketSize)
            {
                return;
            }

            VanDashboardStructs packet{};
            std::memcpy(&packet, message.data, ExpectedPacketSize);

            //carState->IsReverseEngaged = 1;
            carState->IsReverseEngaged    = packet.Field1.data.reverse_gear;
            carState->ExternalTemperature = packet.ExternalTemperature;
            carState->CoolantTemperature  = packet.CoolantTemperature;

            carState->EngineStatus       = packet.Field1.data.engine_running == 1 ? 2 : 0;
            carState->EngineRunning      = packet.Field1.data.engine_running;
            //carState->TrailerPresent     = packet.Field1.trailer_present;
            carState->Ignition           = packet.Field1.data.key_position > 0;
            carState->KeyPosition        = packet.Field1.data.key_position;
            carState->FactoryMode        = packet.Field1.data.factory_mode;

            carState->EconomyMode        = packet.Field1.data.economy_mode;

            carState->Odometer.data.leftByte   = packet.MileageByte1;
            carState->Odometer.data.middleByte = packet.MileageByte2;
            carState->Odometer.data.rightByte  = packet.MileageByte3;

            carState->BlackPanelStatus    = packet.Field0.data.black_panel_status;
            carState->DashboardBrightness = packet.Field0.data.brightness;
            carState->NightMode           = packet.Field0.data.is_backlight_off == 1 ? 0 : 1;

            carState->CarSignalLights.data.parking_light_indicator = carState->NightMode;

            carState->TripOnCMB.asUint24 = (carState->Odometer.asUint24 - carState->MILEAGE_AT_CMB_TRIP_RESET) * 100;

            if (carState->TripOnCMB.asUint24 > 0x1e847f)
            {
                carState->TripOnCMB.asUint24 = 0x1e847f;
            }

            if (carState->Ignition)
            {
                carState->IgnitionMode = 1;
            }
            else
            {
                if (packet.Field1.data.standby_mode)
                {
                    carState->IgnitionMode = 2;
                }
                else
                {
                    carState->IgnitionMode = 0;
                }
            }

            if (_immediateSignalCallback)
            {
                _immediateSignalCallback(ImmediateSignal::ReverseChanged);
            }

            if (_feedbackSignalCallback)
            {
                _feedbackSignalCallback(FeedbackSignal::IgnitionChanged);
            }

            if (carState->Ignition && carState->CurrenTime > 10000 && carState->ExternalTemperature >= 0x4A && carState->ExternalTemperature <= 0x56)
            {
                //show "Ice alert"
                if (!_canPopupHandler->IsPopupVisible())
                {
                    CanDisplayPopupItem item;
                    item.Category = CAN_POPUP_MSG_SHOW_CATEGORY1;
                    item.MessageType = CAN_POPUP_MSG_RISK_OF_ICE;
                    item.DoorStatus1 = 0;
                    item.DoorStatus2 = 0;
                    item.IsInited = false;
                    item.Counter = 0;
                    item.Visible = false;
                    item.SetVisibleOnDisplayTime = 0;
                    item.VANByte = 0;
                    _canPopupHandler->QueueNewMessage(carState->CurrenTime, item);
                }
            }
        }
};
#endif
