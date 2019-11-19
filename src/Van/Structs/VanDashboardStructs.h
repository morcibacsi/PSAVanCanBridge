// VanDashboardStructs.h
#pragma once

#ifndef _VanDashboardStructs_h
#define _VanDashboardStructs_h

// VANID: 8A4
const uint16_t VAN_ID_DASHBOARD = 0x8A4;
const uint8_t VAN_DASHBOARD_LIGHTS_OFF = 0x8F;

// Read right to left in documentation
typedef struct {
    // 0-7
    uint8_t brightness  : 4; // bit 0-3 (0-15, 0x8F when sidelights is off)
    uint8_t heartbeat   : 1; // bit 4
    uint8_t             : 1; // bit 5
    uint8_t             : 1; // bit 6
    uint8_t             : 1; // bit 7
} VanDashboardByte0Struct;


typedef struct {
    uint8_t accesories_on  : 1; // bit 0
    uint8_t ignition_on    : 1; // bit 1
    uint8_t engine_running : 1; // bit 2
    uint8_t door_open_     : 1; // bit 3
    uint8_t economy_mode   : 1; // bit 4
    uint8_t                : 1; // bit 5
    uint8_t                : 1; // bit 6
    uint8_t                : 1; // bit 7
} VanDashboardByte1Struct;

typedef struct {
    uint8_t value : 8;
} VanWaterTemperatureStruct;

typedef struct {
    uint8_t value : 8;
} VanExternalTemperatureStruct;


//Read left to right in documentation
typedef struct VanDashboardStructs {
    VanDashboardByte0Struct Field0;
    VanDashboardByte1Struct Field1;
    VanWaterTemperatureStruct WaterTemperature;
    uint8_t MileageByte1;
    uint8_t MileageByte2;
    uint8_t MileageByte3;
    VanExternalTemperatureStruct ExternalTemperature;
};

typedef union VanDashboardPacket {
    VanDashboardStructs data;
    uint8_t VanDashboardPacket[sizeof(VanDashboardStructs)];
};

int8_t static GetTemperatureToSendToDisplay(int8_t externalTemperature)
{
    return (externalTemperature * 2) + 0x50;
}

int8_t static GetTemperatureFromVANByte(int8_t vanByte)
{
    return (vanByte - 0x50) / 2;
}

int8_t static GetWaterTemperatureFromVANByte(int8_t vanByte)
{
    return (vanByte - 39);
}

int8_t static GetVANByteFromWaterTemperature(int8_t waterTemperature)
{
    return (waterTemperature + 39);
}

#pragma region Sender class
class VanDashboardPacketSender
{
    AbstractVanMessageSender * vanMessageSender;

public:
    VanDashboardPacketSender(AbstractVanMessageSender * object)
    {
        vanMessageSender = object;
    }

    void Send(uint8_t channelId, int8_t externalTemperature, int8_t waterTemperature)
    {
        VanDashboardPacket packet;
        memset(&packet, 0, sizeof(packet));

        packet.data.Field0.brightness = 7;
        packet.data.Field0.heartbeat = 1;
        packet.data.Field1.economy_mode = 0;
        packet.data.Field1.ignition_on = 1;
        packet.data.Field1.accesories_on = 1;
        packet.data.Field1.engine_running = 1;
        packet.data.WaterTemperature.value = GetVANByteFromWaterTemperature(waterTemperature);
        packet.data.MileageByte1 = 0x1A;
        packet.data.MileageByte2 = 0x6E;
        packet.data.MileageByte3 = 0x90; //0x1A6E90 = 173224.0 meters
        packet.data.ExternalTemperature.value = GetTemperatureToSendToDisplay(externalTemperature);

        unsigned char *serializedPacket = Serialize<VanDashboardPacket>(packet);
        vanMessageSender->set_channel_for_transmit_message(channelId, VAN_ID_DASHBOARD, serializedPacket, sizeof(packet), 0);
        memset(&packet, 0, 0);
        delete[] serializedPacket;
    }
};
#pragma endregion

#endif
