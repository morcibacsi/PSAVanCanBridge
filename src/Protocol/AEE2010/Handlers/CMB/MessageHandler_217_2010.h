#pragma once

#ifndef _MessageHandler_217_2010_h
    #define _MessageHandler_217_2010_h

#include <inttypes.h>
#include <memory>
#include "../../../IMessageHandler.hpp"
#include "../../Structs/CAN_217_2010.h"

class MessageHandler_217_2010 : public IMessageHandler<MessageHandler_217_2010>
{
    private:
        BusMessage message
        {
            .id = 0x217,
            .data = {0},
            .dataLength = 8,
            .protocol = ProtocolType::AEE2010,
            .periodicityMs = 100,
            .offsetMs = 25,
            .isActive = true
        };
    public:
        static constexpr uint32_t MessageId = 0x217;

        BusMessage Generate(CarState* carState)
        {
            CAN_217_2010_Byte1Struct byte1{};
            byte1.data.brightness        = carState->OdometerStates.data.brightness;
            byte1.data.black_panel       = carState->OdometerStates.data.black_panel;
            byte1.data.reset_maintenance = carState->OdometerStates.data.reset_maintenance;
            byte1.data.airbag_disabled   = carState->OdometerStates.data.airbag_disabled;

            CAN_217_2010_Byte2Struct byte2{};
            byte2.data.distance_unit = carState->OdometerStates.data.distance_unit;
            byte2.data.reset_trip    = carState->OdometerStates.data.reset_trip;
            byte2.data.reset_trip_1  = carState->OdometerStates.data.reset_trip_1;
            byte2.data.cmb_fault     = carState->OdometerStates.data.cmb_fault;
            byte2.data.auto_check    = carState->OdometerStates.data.auto_check;
            byte2.data.check         = carState->OdometerStates.data.check;
            byte2.data.airbag_fault  = carState->OdometerStates.data.airbag_fault;

            CAN_217_2010_Byte3Struct byte3{};
            byte3.data.aas_pushed                = carState->OdometerStates.data.aas_pushed;
            byte3.data.child_lock_pushed         = carState->OdometerStates.data.child_lock_pushed;
            byte3.data.esp_pushed                = carState->OdometerStates.data.esp_pushed;
            byte3.data.matt_check_request        = carState->OdometerStates.data.matt_check_request;
            byte3.data.partial_filtered_odometer = carState->OdometerStates.data.partial_filtered_odometer;

            CAN_217_2010_Byte4Struct byte4{};
            byte4.data.ac_on_pushed                = carState->OdometerStates.data.ac_on_pushed;
            byte4.data.blindspot_monitor_push      = carState->OdometerStates.data.blindspot_monitor_push;
            byte4.data.compressor_displacement_req = carState->OdometerStates.data.compressor_displacement_req;
            byte4.data.mpd_push                    = carState->OdometerStates.data.mpd_push;
            byte4.data.rear_window_heating_pushed  = carState->OdometerStates.data.rear_window_heating_pushed;
            byte4.data.reset_trip_2                = carState->OdometerStates.data.reset_trip_2;
            byte4.data.stl_pushed                  = carState->OdometerStates.data.stl_pushed;

            CAN_217_2010_Byte5Struct byte5{};
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
        }

        void Parse(CarState* carState, const BusMessage& message)
        {

        }
};
#endif
