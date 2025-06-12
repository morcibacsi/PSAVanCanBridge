#pragma once

#ifndef _MessageHandler_564_h
    #define _MessageHandler_564_h

#include <cstdint>
#include <cstring>

#include "../../Structs/VAN_564.h"
#include "../../../IMessageHandler.hpp"
#include "../../../ImmediateSignal.hpp"
#include "CanDisplayPopupHandler3.hpp"

class MessageHandler_564 : public IMessageHandler<MessageHandler_564>
{
    CanDisplayPopupHandler3* _canPopupHandler;
    ImmediateSignalCallback _immediateSignalCallback;

    public:
        static constexpr uint32_t MessageId = 0x564;

        MessageHandler_564()
        {
        }

        void SetImmediateSignalCallback(ImmediateSignalCallback immediateSignalCallback) { _immediateSignalCallback = immediateSignalCallback; }
        void SetCanDisplayPopupHandler(CanDisplayPopupHandler3* canPopupHandler) { _canPopupHandler = canPopupHandler; }

        BusMessage Generate(CarState* carState)
        {
            BusMessage message;
            message.id = MessageId;
            message.periodicityMs = 500;
            message.offsetMs = 20;
            message.protocol = ProtocolType::AEE2001;
            message.type = MessageType::Query;
            message.isActive = false; // it is sent as a feedback message when 8C4 is received

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message)
        {
            constexpr std::size_t ExpectedPacketSize = sizeof(VanCarStatusWithTripComputerStruct);
            //printf("MessageHandler_564::Parse\n");

            if (message.dataLength != ExpectedPacketSize)
            {
                return;
            }

            VanCarStatusWithTripComputerStruct packet{};
            std::memcpy(&packet, message.data, ExpectedPacketSize);

            carState->DoorStatus.data.front_left_door_open  = packet.Doors.FrontLeft;
            carState->DoorStatus.data.front_right_door_open = packet.Doors.FrontRight;
            carState->DoorStatus.data.rear_left_door_open   = packet.Doors.RearLeft;
            carState->DoorStatus.data.rear_right_door_open  = packet.Doors.RearRight;
            carState->DoorStatus.data.trunk_open            = packet.Doors.BootLid;
            carState->DoorStatus.data.hood_open             = packet.Doors.Hood;
            carState->DoorStatus.data.fuel_flap_open        = packet.Doors.FuelFlap;
            carState->DoorStatus.data.rear_window_open      = packet.Doors.Sunroof;

            CanDisplayDoorStatus1Struct doorStatus{};
            doorStatus.asByte = 0;
            doorStatus.data.is_front_left_open  = packet.Doors.FrontLeft;
            doorStatus.data.is_front_right_open = packet.Doors.FrontRight;
            doorStatus.data.is_rear_left_open   = packet.Doors.RearLeft;
            doorStatus.data.is_rear_right_open  = packet.Doors.RearRight;
            doorStatus.data.is_boot_open        = packet.Doors.BootLid;
            doorStatus.data.is_bonnet_open      = packet.Doors.Hood;

            CanDisplayDoorStatus2Struct doorStatus2{};
            doorStatus2.asByte = 0;
            doorStatus2.data.is_fuel_flap_open = packet.Doors.FuelFlap;

            CanDisplayPopupItem item{};
            item.Category = CAN_POPUP_MSG_SHOW_CATEGORY1;
            item.MessageType = CAN_POPUP_MSG_DOORS_BOOT_BONNET_REAR_SCREEN_AND_FUEL_TANK_OPEN;
            item.DoorStatus1 = doorStatus.asByte;
            item.DoorStatus2 = doorStatus2.asByte;
            item.KmToDisplay = 0;
            item.IsInited = false;
            item.Counter = 0;
            item.Visible = false;
            item.SetVisibleOnDisplayTime = 0;
            item.VANByte = 0x02;
            _canPopupHandler->QueueNewMessage(carState->CurrenTime, item);

            //221
            carState->RightStickButtonPushed = packet.Field10.TripButton;
            //printf("MessageHandler_564::Parse - RightStickButtonPushed: %d\n", carState->RightStickButtonPushed);

            carState->RemainingRange.data.leftByte  = packet.FuelLeftToPumpInKm1;
            carState->RemainingRange.data.rightByte = packet.FuelLeftToPumpInKm2;

            carState->InstantConsumption.data.leftByte  = packet.FuelConsumption1;
            carState->InstantConsumption.data.rightByte = packet.FuelConsumption2;

            carState->TotalRange.data.leftByte  = packet.Trip1Distance1;
            carState->TotalRange.data.rightByte = packet.Trip1Distance2;

            //2A1
            carState->Trip1Speed = packet.AvgCourseSpeed;

            carState->Trip1Distance.data.leftByte  = packet.Trip1Distance1;
            carState->Trip1Distance.data.rightByte = packet.Trip1Distance2;

            carState->Trip1Consumption.data.leftByte  = packet.Trip1FuelConsumption1;
            carState->Trip1Consumption.data.rightByte = packet.Trip1FuelConsumption2;

            //261
            carState->Trip2Speed = packet.AvgTravelSpeed;

            carState->Trip2Distance.data.leftByte  = packet.Trip2Distance1;
            carState->Trip2Distance.data.rightByte = packet.Trip2Distance2;

            carState->Trip2Consumption.data.leftByte  = packet.Trip2FuelConsumption1;
            carState->Trip2Consumption.data.rightByte = packet.Trip2FuelConsumption2;

            if (_immediateSignalCallback)
            {
                //printf("MessageHandler_564::Parse - ImmediateSignal::TripButtonPressed\n");
                _immediateSignalCallback(ImmediateSignal::TripButtonPressed);
            }
        }
};
#endif
