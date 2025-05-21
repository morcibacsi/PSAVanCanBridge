#pragma once

#ifndef _MessageHandler_217_h
    #define _MessageHandler_217_h

#include <inttypes.h>
#include <memory>
#include <cstring>
#include "../../../IMessageHandler.hpp"
#include "../../Structs/CAN_217.h"

class MessageHandler_217 : public IMessageHandler<MessageHandler_217>
{
    private:
        BusMessage message
        {
            .id = 0x217,
            .data = {0},
            .dataLength = 8,
            .protocol = ProtocolType::AEE2004,
            .periodicityMs = 100,
            .offsetMs = 25,
            //TODO
            .isActive = false
        };

        ImmediateSignalCallback _immediateSignalCallback;

    public:
        static constexpr uint32_t MessageId = 0x217;

        MessageHandler_217()
        {
        }

        void SetImmediateSignalCallback(ImmediateSignalCallback immediateSignalCallback) { _immediateSignalCallback = immediateSignalCallback; }

        BusMessage Generate(CarState* carState)
        {
            //TODO need to add a config flag to enable/disable this message
            CAN_217_Byte1Struct byte1{};
            byte1.data.brightness        = carState->OdometerStates.data.brightness;
            byte1.data.black_panel       = carState->OdometerStates.data.black_panel;
            byte1.data.reset_maintenance = carState->OdometerStates.data.reset_maintenance;
            byte1.data.airbag_disabled   = carState->OdometerStates.data.airbag_disabled;

            CAN_217_Byte2Struct byte2{};
            byte2.data.distance_unit = carState->OdometerStates.data.distance_unit;
            byte2.data.reset_trip    = carState->OdometerStates.data.reset_trip;
            byte2.data.reset_trip_1  = carState->OdometerStates.data.reset_trip_1;
            byte2.data.cmb_fault     = carState->OdometerStates.data.cmb_fault;
            byte2.data.auto_check    = carState->OdometerStates.data.auto_check;
            byte2.data.check         = carState->OdometerStates.data.check;
            byte2.data.airbag_fault  = carState->OdometerStates.data.airbag_fault;

            CAN_217_Byte3Struct byte3{};
            byte3.data.aas_pushed                = carState->OdometerStates.data.aas_pushed;
            byte3.data.child_lock_pushed         = carState->OdometerStates.data.child_lock_pushed;
            byte3.data.esp_pushed                = carState->OdometerStates.data.esp_pushed;
            byte3.data.matt_check_request        = carState->OdometerStates.data.matt_check_request;
            byte3.data.partial_filtered_odometer = carState->OdometerStates.data.partial_filtered_odometer;

            CAN_217_Byte4Struct byte4{};
            byte4.data.ac_on_pushed                = carState->OdometerStates.data.ac_on_pushed;
            byte4.data.blindspot_monitor_push      = carState->OdometerStates.data.blindspot_monitor_push;
            byte4.data.compressor_displacement_req = carState->OdometerStates.data.compressor_displacement_req;
            byte4.data.mpd_push                    = carState->OdometerStates.data.mpd_push;
            byte4.data.rear_window_heating_pushed  = carState->OdometerStates.data.rear_window_heating_pushed;
            byte4.data.reset_trip_2                = carState->OdometerStates.data.reset_trip_2;
            byte4.data.stl_pushed                  = carState->OdometerStates.data.stl_pushed;

            CAN_217_Byte5Struct byte5{};
            byte5.data.artiv_minus             = carState->OdometerStates.data.artiv_minus;
            byte5.data.artiv_plus              = carState->OdometerStates.data.artiv_plus;
            byte5.data.artiv_push              = carState->OdometerStates.data.artiv_push;
            byte5.data.reset_maintenance_matt  = carState->OdometerStates.data.reset_maintenance_matt;

            message.data[0] = byte1.asByte;
            message.data[1] = byte2.asByte;
            message.data[2] = byte3.asByte;
            message.data[3] = byte4.asByte;
            message.data[4] = byte5.asByte;
            message.data[5] = carState->OdometerStates.data.SpeedDisplayedOnCmb;
            message.data[6] = 0x00;
            message.data[7] = 0x00;

            return message;
            //*/
        }

        void Parse(CarState* carState, const BusMessage& message)
        {
            CAN_217Struct packet;
            std::memcpy(&packet, message.data, sizeof(packet));

            carState->OdometerStates.data.SpeedDisplayedOnCmb = packet.SpeedDisplayedOnCmb;
            carState->OdometerStates.data.brightness          = packet.Brightness.data.brightness;
            carState->OdometerStates.data.black_panel         = packet.Brightness.data.brightness;
            carState->OdometerStates.data.reset_maintenance   = packet.Brightness.data.reset_maintenance;

            carState->OdometerStates.data.airbag_fault  = packet.Field2.data.airbag_fault;
            carState->OdometerStates.data.auto_check    = packet.Field2.data.auto_check;
            carState->OdometerStates.data.check         = packet.Field2.data.check;
            carState->OdometerStates.data.cmb_fault     = packet.Field2.data.cmb_fault;
            //TODO check if we can use in settings
            carState->OdometerStates.data.distance_unit = packet.Field2.data.distance_unit;
            carState->OdometerStates.data.reset_trip    = packet.Field2.data.reset_trip;
            carState->OdometerStates.data.reset_trip_1  = packet.Field2.data.reset_trip_1;

            carState->OdometerStates.data.aas_pushed                = packet.Field3.data.aas_pushed;
            carState->OdometerStates.data.child_lock_pushed         = packet.Field3.data.child_lock_pushed;
            carState->OdometerStates.data.esp_pushed                = packet.Field3.data.esp_pushed;
            carState->OdometerStates.data.matt_check_request        = packet.Field3.data.matt_check_request;
            carState->OdometerStates.data.partial_filtered_odometer = packet.Field3.data.partial_filtered_odometer;
            carState->OdometerStates.data.sport_push                = packet.Field3.data.sport_push;

            carState->OdometerStates.data.ac_on_pushed                = packet.Field4.data.ac_on_pushed;
            carState->OdometerStates.data.blindspot_monitor_push      = packet.Field4.data.blindspot_monitor_push;
            carState->OdometerStates.data.compressor_displacement_req = packet.Field4.data.compressor_displacement_req;
            carState->OdometerStates.data.mpd_push                    = packet.Field4.data.mpd_push;
            carState->OdometerStates.data.rear_window_heating_pushed  = packet.Field4.data.rear_window_heating_pushed;
            carState->OdometerStates.data.reset_trip_2                = packet.Field4.data.reset_trip_2;
            carState->OdometerStates.data.stl_pushed                  = packet.Field4.data.stl_pushed;

            carState->OdometerStates.data.artiv_minus                 = packet.Field5.data.artiv_minus;
            carState->OdometerStates.data.artiv_plus                  = packet.Field5.data.artiv_plus;
            carState->OdometerStates.data.artiv_push                  = packet.Field5.data.artiv_push;
            carState->OdometerStates.data.reset_maintenance_matt      = packet.Field5.data.reset_maintenance_matt;
            carState->OdometerStates.data.stt_push                    = packet.Field5.data.stt_pushed;

            if (carState->MODIFY_217_WITH_CURRENT_SPEED)
            {
                carState->OdometerStates.data.SpeedDisplayedOnCmb = carState->SpeedInKmh;
            }

            if (_immediateSignalCallback)
            {
                _immediateSignalCallback(ImmediateSignal::CmbStatusChanged);
            }
        }
};
#endif
