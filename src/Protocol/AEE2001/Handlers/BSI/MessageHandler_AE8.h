#pragma once

#ifndef _MessageHandler_AE8_h
    #define _MessageHandler_AE8_h

#include <inttypes.h>
#include <memory>
#include <cstring>

#include "../../Structs/VAN_8A4.h"
#include "../../../IMessageHandler.hpp"

class MessageHandler_AE8 : public IMessageHandler
{
    public:
        BusMessage Generate(CarState* state) override
        {
            BusMessage message;
            message.id = 0xAE8;
            message.periodicityMs = 500;
            message.offsetMs = 20;
            message.protocol = ProtocolType::AEE2001;
            message.type = MessageType::Query;
            message.isActive = false; //it is sent as a feedback message when A68 is received

            return message;
        }

        void Parse(CarState* carState, const BusMessage& message) override
        {
            if (message.dataLength != 24 || carState->PARKING_AID_TYPE != 1)
            {
                return;
            }

            if (message.data[2] == 0xA0)
            {
                carState->HasParkingRadarData = 1;
                uint8_t enableSoundFromSpeaker = 0;
                if (carState->ENABLE_PARKING_AID_SOUND_FROM_SPEAKER)
                {
                    enableSoundFromSpeaker = 1;
                }
                carState->ParkingSoundEnabled      = enableSoundFromSpeaker;
                carState->ParkingExteriorRearLeft  = message.data[3];
                carState->ParkingExteriorRearRight = message.data[4];
                carState->ParkingInteriorRearLeft  = message.data[5];
                carState->ParkingInteriorRearRight = message.data[6];
            }
        }
};
#endif
