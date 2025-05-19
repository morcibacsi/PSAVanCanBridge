#pragma once

#ifndef _MessageHandler_8A4_h
    #define _MessageHandler_8A4_h

#include <inttypes.h>
#include <memory>
#include <cstring>

#include "../../Structs/VAN_8A4.h"
#include "../../../IMessageHandler.hpp"
#include "CanDisplayPopupHandler3.h"

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

        BusMessage Generate(CarState* state)
        {
            BusMessage message;
            message.id = 0x8A4;
            message.periodicityMs = 500;
            message.offsetMs = 20;
            message.protocol = ProtocolType::AEE2001;

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {
            constexpr std::size_t ExpectedPacketSize = sizeof(VanDashboardStructs);

            if (message.dataLength != ExpectedPacketSize)
            {
                return;
            }

            VanDashboardStructs vanPacket{};
            std::memcpy(&vanPacket, message.data, ExpectedPacketSize);

            carState->Ignition = vanPacket.Field1.ignition_on || vanPacket.Field1.accesories_on || vanPacket.Field1.engine_running;
            carState->ExternalTemperature = vanPacket.ExternalTemperature;
            carState->CoolantTemperature = vanPacket.CoolantTemperature;
            carState->EngineRunning = vanPacket.Field1.engine_running;

            carState->IsReverseEngaged = vanPacket.Field1.reverse_gear;
            //carState->IsReverseEngaged = 1;
            carState->TrailerPresent = vanPacket.Field1.trailer_present;
            carState->EconomyMode = vanPacket.Field1.economy_mode;
            //carState->HasParkingRadarData = vanPacket.Field1.reverse_gear;

            carState->Odometer.data.leftByte = vanPacket.MileageByte1;
            carState->Odometer.data.middleByte = vanPacket.MileageByte2;
            carState->Odometer.data.rightByte = vanPacket.MileageByte3;

            carState->BlackPanelStatus = vanPacket.Field0.black_panel_status;
            carState->DashboardBrightness = vanPacket.Field0.brightness;
            carState->NightMode = vanPacket.Field0.is_backlight_off == 1 ? 0 : 1;
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
                if (vanPacket.Field1.standby_mode)
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
