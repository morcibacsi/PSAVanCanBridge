// VanDisplayStatusStructs.h
#pragma once

#ifndef _VanDisplayStatusStructs_h
    #define _VanDisplayStatusStructs_h

// VANID: 5E4
const uint16_t VAN_ID_DISPLAY_STATUS = 0x5E4;
const uint8_t VAN_ID_EMF_BSI_REQUEST_LENGTH = 2;

typedef struct {
    uint8_t unused                             : 2; // bit 0-1
    uint8_t alert_reminder_request             : 1; // bit 2
    uint8_t overspeed_memorization_request     : 1; // bit 3
    uint8_t overspeed_alert                    : 1; // bit 4
    uint8_t request_for_keep_van_comfort_alive : 1; // bit 5
    uint8_t request_to_reset_cumulative        : 1; // bit 6
    uint8_t request_to_reset_course_totals     : 1; // bit 7
} VanEmfBsiRequestByte1;

// Read left to right in documentation
typedef struct VanDisplayStatusStruct {
    VanEmfBsiRequestByte1 Requests;
    uint8_t OverSpeedAlertValue;
};

typedef union VanDisplayStatusPacket {
    VanDisplayStatusStruct data;
    uint8_t VanDisplayStatusPacket[sizeof(VanDisplayStatusStruct)];
};

#pragma region Sender class

#include "../AbstractVanMessageSender.h"
#include "../../Helpers/Serializer.h"

class VanDisplayStatusPacketSender
{
    AbstractVanMessageSender * vanMessageSender;

public:
    VanDisplayStatusPacketSender(AbstractVanMessageSender * object)
    {
        vanMessageSender = object;
    }

    void SendStatus(uint8_t channelId, uint8_t resetTrip)
    {
        VanDisplayStatusPacket packet;
        memset(&packet, 0, sizeof(packet));

        packet.data.Requests.request_for_keep_van_comfort_alive = 1;
        packet.data.Requests.request_to_reset_course_totals = resetTrip;

        packet.data.OverSpeedAlertValue = 0x1E;

        unsigned char *serializedPacket = Serialize<VanDisplayStatusPacket>(packet);
        vanMessageSender->set_channel_for_transmit_message(channelId, VAN_ID_DISPLAY_STATUS, serializedPacket, sizeof(packet), 1);
        memset(&packet, 0, 0);
        delete[] serializedPacket;
    }

    void Disable(uint8_t channelId)
    {
        vanMessageSender->disable_channel(channelId);
    }
};
#pragma endregion

#endif
