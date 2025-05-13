#pragma once

#ifndef _MessageHandler_564_h
    #define _MessageHandler_564_h

#include <inttypes.h>
#include <memory>
#include <cstring>

#include "../../Structs/VAN_564.h"
#include "../../../IMessageHandler.hpp"

class MessageHandler_564 : public IMessageHandler
{
    std::shared_ptr<CanDisplayPopupHandler3> _canPopupHandler;
    std::function<void(ImmediateSignal)> _immediateSignalCallback;

    public:
        MessageHandler_564(
            std::shared_ptr<CanDisplayPopupHandler3> canPopupHandler,
            std::function<void(ImmediateSignal)> immediateSignalCallback
        )
        {
            _immediateSignalCallback = immediateSignalCallback;
            _canPopupHandler = std::move(canPopupHandler);
        }

        BusMessage Generate(std::shared_ptr<CarState> state) override
        {
            BusMessage message;
            message.id = 0x564;
            message.periodicityMs = 500;
            message.offsetMs = 20;
            message.protocol = ProtocolType::AEE2001;

            return message;
        }

        void Parse(std::shared_ptr<CarState> carState, const BusMessage& message) override
        {
            constexpr std::size_t ExpectedPacketSize = sizeof(VanCarStatusWithTripComputerStruct);
            //printf("MessageHandler_564::Parse\n");

            if (message.dataLength != ExpectedPacketSize)
            {
                return;
            }

            VanCarStatusWithTripComputerStruct vanPacket{};
            std::memcpy(&vanPacket, message.data, ExpectedPacketSize);

            carState->DoorStatus.data.front_left_door_open  = vanPacket.Doors.FrontLeft;
            carState->DoorStatus.data.front_right_door_open = vanPacket.Doors.FrontRight;
            carState->DoorStatus.data.rear_left_door_open   = vanPacket.Doors.RearLeft;
            carState->DoorStatus.data.rear_right_door_open  = vanPacket.Doors.RearRight;
            carState->DoorStatus.data.trunk_open            = vanPacket.Doors.BootLid;
            carState->DoorStatus.data.hood_open             = vanPacket.Doors.Hood;
            carState->DoorStatus.data.fuel_flap_open        = vanPacket.Doors.FuelFlap;
            carState->DoorStatus.data.rear_window_open      = vanPacket.Doors.Sunroof;

            CanDisplayDoorStatus1Struct doorStatus{};
            doorStatus.asByte = 0;
            doorStatus.data.is_front_left_open  = vanPacket.Doors.FrontLeft;
            doorStatus.data.is_front_right_open = vanPacket.Doors.FrontRight;
            doorStatus.data.is_rear_left_open   = vanPacket.Doors.RearLeft;
            doorStatus.data.is_rear_right_open  = vanPacket.Doors.RearRight;
            doorStatus.data.is_boot_open        = vanPacket.Doors.BootLid;
            doorStatus.data.is_bonnet_open      = vanPacket.Doors.Hood;

            CanDisplayDoorStatus2Struct doorStatus2{};
            doorStatus2.asByte = 0;
            doorStatus2.data.is_fuel_flap_open = vanPacket.Doors.FuelFlap;

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
            carState->RightStickButtonPushed = vanPacket.Field10.TripButton;
            //printf("MessageHandler_564::Parse - RightStickButtonPushed: %d\n", carState->RightStickButtonPushed);

            carState->RemainingRange.data.leftByte  = vanPacket.FuelLeftToPumpInKm1;
            carState->RemainingRange.data.rightByte = vanPacket.FuelLeftToPumpInKm2;

            carState->InstantConsumption.data.leftByte  = vanPacket.FuelConsumption1;
            carState->InstantConsumption.data.rightByte = vanPacket.FuelConsumption2;

            carState->TotalRange.data.leftByte  = vanPacket.Trip1Distance1;
            carState->TotalRange.data.rightByte = vanPacket.Trip1Distance2;

            //2A1
            carState->Trip1Speed = vanPacket.AvgCourseSpeed;

            carState->Trip1Distance.data.leftByte  = vanPacket.Trip1Distance1;
            carState->Trip1Distance.data.rightByte = vanPacket.Trip1Distance2;

            carState->Trip1Consumption.data.leftByte  = vanPacket.Trip1FuelConsumption1;
            carState->Trip1Consumption.data.rightByte = vanPacket.Trip1FuelConsumption2;

            //261
            carState->Trip2Speed = vanPacket.AvgTravelSpeed;

            carState->Trip2Distance.data.leftByte  = vanPacket.Trip2Distance1;
            carState->Trip2Distance.data.rightByte = vanPacket.Trip2Distance2;

            carState->Trip2Consumption.data.leftByte  = vanPacket.Trip2FuelConsumption1;
            carState->Trip2Consumption.data.rightByte = vanPacket.Trip2FuelConsumption2;

            if (_immediateSignalCallback)
            {
                //printf("MessageHandler_564::Parse - ImmediateSignal::TripButtonPressed\n");
                _immediateSignalCallback(ImmediateSignal::TripButtonPressed);
            }
        }
};
#endif
