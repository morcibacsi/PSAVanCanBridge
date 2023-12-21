#include "VanACDiagPacketSender.h"
#include "../Structs/VanAirConditionerDiagStructs.h"

VanACDiagPacketSender::VanACDiagPacketSender(IVanMessageSender* object)
{
    vanMessageSender = object;
}

void VanACDiagPacketSender::GetManufacturerInfo(uint8_t channelId)
{
    uint8_t packet[2] = { 0x21, 0x80 };
    vanMessageSender->set_channel_for_transmit_message(channelId, VAN_ID_AIR_CONDITIONER_DIAG_COMMAND, packet, 2, 1);
}

void VanACDiagPacketSender::GetSettings(uint8_t channelId)
{
    uint8_t packet[2] = { 0x21, 0xA0 };
    vanMessageSender->set_channel_for_transmit_message(channelId, VAN_ID_AIR_CONDITIONER_DIAG_COMMAND, packet, 2, 1);
}

void VanACDiagPacketSender::GetSensorStatus(uint8_t channelId)
{
    uint8_t packet[2] = { 0x21, 0xC0 };
    vanMessageSender->set_channel_for_transmit_message(channelId, VAN_ID_AIR_CONDITIONER_DIAG_COMMAND, packet, 2, 1);
}

void VanACDiagPacketSender::GetActuatorStatus(uint8_t channelId)
{
    uint8_t packet[2] = { 0x21, 0xC1 };
    vanMessageSender->set_channel_for_transmit_message(channelId, VAN_ID_AIR_CONDITIONER_DIAG_COMMAND, packet, 2, 1);
}

void VanACDiagPacketSender::GetButtonStatus(uint8_t channelId)
{
    uint8_t packet[2] = { 0x21, 0xC2 };
    vanMessageSender->set_channel_for_transmit_message(channelId, VAN_ID_AIR_CONDITIONER_DIAG_COMMAND, packet, 2, 1);
}

void VanACDiagPacketSender::GetFaultCodes(uint8_t channelId)
{
    uint8_t packet[2] = { 0x21, 0xC4 };
    vanMessageSender->set_channel_for_transmit_message(channelId, VAN_ID_AIR_CONDITIONER_DIAG_COMMAND, packet, 2, 1);
}

void VanACDiagPacketSender::QueryAirConData(uint8_t channelId)
{
    vanMessageSender->set_channel_for_reply_request_message(channelId, VAN_ID_AIR_CONDITIONER_DIAG, 26 + 2, 1);
}
