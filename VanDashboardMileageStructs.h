// VanDashboardMileageStructs.h
#pragma once

#ifndef _VanDashboardMileageStructs_h
    #define _VanDashboardMileageStructs_h

// VANID: 8FC
const uint16_t VAN_ID_DASHBOARD_MILEAGE = 0x8FC;


// Read left to right in documentation
typedef struct VanDashboardMileageStructs {
    uint8_t Header;
    uint8_t MileageByte1;
    uint8_t MileageByte2;
    uint8_t MileageByte3;
    uint8_t Footer;
};

typedef union VanDashboardMileagePacket {
    VanDashboardMileageStructs data;
    uint8_t VanDashboardMileagePacket[sizeof(VanDashboardMileageStructs)];
};

#pragma region Sender class
class VanDashboardMileagePacketSender
{
    AbstractVanMessageSender* vanMessageSender;
public:
    VanDashboardMileagePacketSender(AbstractVanMessageSender* object)
    {
        vanMessageSender = object;
    }

    void GetMileageFromDashboard(uint8_t channelId)
    {
        vanMessageSender->set_channel_for_reply_request_message(channelId, VAN_ID_DASHBOARD_MILEAGE, 7, 1);
    }
};
#pragma endregion


#endif
