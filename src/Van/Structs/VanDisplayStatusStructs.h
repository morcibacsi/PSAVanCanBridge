// VanDisplayStatusStructs.h
#pragma once

#ifndef _VanDisplayStatusStructs_h
    #define _VanDisplayStatusStructs_h

#include "../../Helpers/Serializer.h"
#include "../AbstractVanMessageSender.h"

// VANID: 5E4
const uint16_t VAN_ID_DISPLAY_STATUS = 0x5E4;

// Read left to right in documentation
typedef struct VanDisplayStatusStruct {
    uint8_t Byte0;
    uint8_t Byte1;
};

typedef union VanDisplayStatusPacket {
    VanDisplayStatusStruct data;
    uint8_t VanDisplayStatusPacket[sizeof(VanDisplayStatusStruct)];
};


#pragma region Sender class
class VanDisplayStatusPacketSender
{
    AbstractVanMessageSender * vanMessageSender;

public:
    VanDisplayStatusPacketSender(AbstractVanMessageSender * object)
    {
        vanMessageSender = object;
    }

    void SendReady(uint8_t channelId)
    {
        VanDisplayStatusPacket packet;
        memset(&packet, 0, sizeof(packet));

        packet.data.Byte0 = 0x20;
        packet.data.Byte1 = 0x1E;

        unsigned char *serializedPacket = Serialize<VanDisplayStatusPacket>(packet);
        vanMessageSender->set_channel_for_transmit_message(channelId, VAN_ID_DISPLAY_STATUS, serializedPacket, sizeof(packet), 1);
        memset(&packet, 0, 0);
        delete[] serializedPacket;
    }

    void SendTripReset(uint8_t channelId)
    {
        VanDisplayStatusPacket packet;
        memset(&packet, 0, sizeof(packet));

        packet.data.Byte0 = 0xA0;
        packet.data.Byte1 = 0x1E;

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
