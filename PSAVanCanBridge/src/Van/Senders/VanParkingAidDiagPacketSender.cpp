#include "../Structs/VanParkingAidDiagStructs.h"
#include "VanParkingAidDiagPacketSender.h"

VanParkingAidDiagPacketSender::VanParkingAidDiagPacketSender(IVanMessageSender* object)
{
    vanMessageSender = object;
}

void VanParkingAidDiagPacketSender::GetManufacturerInfo(uint8_t channelId)
{
    uint8_t packet[2] = { 0x21, 0x80 };
    vanMessageSender->set_channel_for_transmit_message(channelId, VAN_ID_PARKING_AID_DIAG_QUERY, packet, 2, 1);
}

void VanParkingAidDiagPacketSender::GetDistance(uint8_t channelId)
{
    uint8_t packet[2] = { 0x21, 0xA0 };
    vanMessageSender->set_channel_for_transmit_message(channelId, VAN_ID_PARKING_AID_DIAG_QUERY, packet, 2, 1);
}

void VanParkingAidDiagPacketSender::GetConfiguration(uint8_t channelId)
{
    uint8_t packet[2] = { 0x21, 0xC0 };
    vanMessageSender->set_channel_for_transmit_message(channelId, VAN_ID_PARKING_AID_DIAG_QUERY, packet, 2, 1);
}

void VanParkingAidDiagPacketSender::GetInputStates(uint8_t channelId)
{
    uint8_t packet[2] = { 0x21, 0xC5 };
    vanMessageSender->set_channel_for_transmit_message(channelId, VAN_ID_PARKING_AID_DIAG_QUERY, packet, 2, 1);
}

void VanParkingAidDiagPacketSender::ActivateBuzzer(uint8_t channelId)
{
    uint8_t packet[3] = { 0x30, 0x80, 0x01 };
    vanMessageSender->set_channel_for_transmit_message(channelId, VAN_ID_PARKING_AID_DIAG_QUERY, packet, 3, 1);
}

void VanParkingAidDiagPacketSender::GetFaultCodes(uint8_t channelId)
{
    uint8_t packet[2] = { 0x21, 0x83 };
    vanMessageSender->set_channel_for_transmit_message(channelId, VAN_ID_PARKING_AID_DIAG_QUERY, packet, 2, 1);
}

void VanParkingAidDiagPacketSender::FaultClearing(uint8_t channelId)
{
    uint8_t packet[2] = { 0x14, 0xFF };
    vanMessageSender->set_channel_for_transmit_message(channelId, VAN_ID_PARKING_AID_DIAG_QUERY, packet, 2, 1);
}

void VanParkingAidDiagPacketSender::QueryParkingRadarData(uint8_t channelId)
{
    //the longest message should fit into the buffer (fault reading 28 + 2 CRC)
    vanMessageSender->set_channel_for_reply_request_message(channelId, VAN_ID_PARKING_AID_DIAG_ANSWER, 28 + 2, 1);
}
